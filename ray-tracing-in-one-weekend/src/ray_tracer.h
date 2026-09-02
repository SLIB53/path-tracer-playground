#pragma once

#include <print>
#include <thread>

#include "camera.h"
#include "shape.h"

class ray_tracer {
public:
  unsigned image_width = 0, image_height = 0;
  unsigned samples_per_pixel = 0;
  unsigned trace_depth_maximum = 0;

  void render(const camera &main_camera, const shape_range auto &world,
              std::vector<color> &out_imagebuffer) const {
    constexpr unsigned tile_width = 32, tile_height = 32;

    const unsigned tile_row_count =
                       (image_height + tile_height - 1) / tile_height,
                   tile_column_count =
                       (image_width + tile_width - 1) / tile_width;

    const unsigned tile_total_count = tile_row_count * tile_column_count;

    std::atomic<unsigned> tiles_provisioned{0}, tiles_completed{0};

    const auto begin_tile_renderer = [&] {
      while (true) {
        const auto t =
            tiles_provisioned.fetch_add(1, std::memory_order_relaxed);
        if (t >= tile_total_count)
          break;

        const std::size_t t_row_start = (t / tile_column_count) * tile_height,
                          t_column_start = (t % tile_column_count) * tile_width;
        const std::size_t t_row_end =
                              std::min(t_row_start + tile_height,
                                       static_cast<std::size_t>(image_height)),
                          t_column_end =
                              std::min(t_column_start + tile_width,
                                       static_cast<std::size_t>(image_width));

        for (auto row = t_row_start; row < t_row_end; ++row)
          for (auto column = t_column_start; column < t_column_end; ++column)
            out_imagebuffer[row * image_width + column] = linear_to_gamma_color(
                super_sampled_pixel_color(main_camera, world, row, column));

        tiles_completed.fetch_add(1, std::memory_order_relaxed);
      }
    };

    const auto begin_progress_reporter =
        [&tiles_completed, &tile_total_count](const std::stop_token stop) {
          while (!stop.stop_requested()) {
            auto completed = tiles_completed.load(std::memory_order_relaxed);
            std::print(stderr, "\r\x1b[K{:.0f}% ({}/{} tiles)",
                       double(completed) / tile_total_count * 100.0, completed,
                       tile_total_count);

            std::this_thread::sleep_for(std::chrono::seconds(1));
          }
        };

    out_imagebuffer.resize(image_width * image_height);

    std::jthread progress_reporter{begin_progress_reporter};

    {
      const auto tile_renderer_count = std::max(
          static_cast<unsigned>(1), std::thread::hardware_concurrency());

      std::vector<std::jthread> tile_renderers;
      tile_renderers.reserve(tile_renderer_count);
      for (unsigned i = 0; i < tile_renderer_count; ++i)
        tile_renderers.emplace_back(begin_tile_renderer);
    }

    std::println(stderr, "\r\x1b[K{:.0f}% ({}/{} tiles)", 100.0,
                 tile_total_count, tile_total_count);
  }

private:
  // Given a trace ray, interpolate a gradient.
  color background_color(const ray_3 &trace_ray) const {
    auto a = (normalize(trace_ray.direction()).y() + 1.0) / 2.0;

    color result = (1.0 - a) * color(1.0, 1.0, 1.0) + a * base16_palette[8];
    assert_color(result);

    return result;
  }

  // Given (u, v) coordinates of a viewport, sample the world from the
  // coordinates.
  color trace_color(const camera &main_camera, const shape_range auto &world,
                    double u, double v) const {
    ray_3 trace_tail;
    {
      auto pixel_delta_u = main_camera.viewport_u() / image_width,
           pixel_delta_v = main_camera.viewport_v() / image_height;

      auto pixel_00_center =
          main_camera.viewport_origin() + (pixel_delta_u + pixel_delta_v) / 2.0;

      auto pixel_uv_center =
          pixel_00_center + (u * pixel_delta_u) + (v * pixel_delta_v);

      auto depth_of_field_disk_r = vector_3::random_on_unit_disk();
      auto depth_of_field_disk_uv =
          depth_of_field_disk_r[0] * main_camera.depth_of_field_disk_u() +
          depth_of_field_disk_r[1] * main_camera.depth_of_field_disk_v();

      point_3 trace_tail_origin =
          main_camera.axial_ray.origin() + depth_of_field_disk_uv;
      vector_3 trace_tail_direction = pixel_uv_center - trace_tail_origin;
      trace_tail = ray_3(trace_tail_origin, trace_tail_direction);
    }

    color trace_accumulation;
    {
      // Initial value of (1, 1, 1) is fine because it won't contribute in
      // pairwise multiplication.
      trace_accumulation = color(1.0, 1.0, 1.0);

      // Because the accumulation is commutative, we can use the iterative form
      // without keeping a stack rather than accumulate recursively.
      for (unsigned trace_depth = 0; trace_depth < trace_depth_maximum;
           ++trace_depth) {
        ray_3_intersection intersection;
        if (!intersects(world, trace_tail, interval(0.001, +infinity),
                        intersection))
          break;

        color attenuation;
        ray_3 scattered_ray;
        if (!intersection.surface_material->scatter(trace_tail, intersection,
                                                    attenuation, scattered_ray))
          break;

        assert(!approximately_equals(scattered_ray.direction(), vector_3()));

        trace_tail = scattered_ray;
        trace_accumulation =
            element_wise_product(trace_accumulation, attenuation);
      }
    }

    // Assume the final ray reaches the background (even though this may not
    // actually be the case), and multiply the background color. This will treat
    // the background color as the ambient light.
    color result =
        element_wise_product(trace_accumulation, background_color(trace_tail));
    assert_color(result);

    return result;
  }

  // Given the row and column of the image, average samples of the world from
  // the image's pixel at the row and column.
  color super_sampled_pixel_color(const camera &main_camera,
                                  const shape_range auto &world, unsigned row,
                                  unsigned column) const {
    vector_3 sample_color_sum;
    for (unsigned sample = 0; sample < samples_per_pixel; ++sample) {
      auto u_jittered = column + random_in_interval(zero_to_one) - 0.5,
           v_jittered = row + random_in_interval(zero_to_one) - 0.5;

      sample_color_sum +=
          trace_color(main_camera, world, u_jittered, v_jittered);
    }

    color result = sample_color_sum / samples_per_pixel;
    assert_color(result);

    return result;
  }
};
