#pragma once

#include <print>

#include "camera.h"
#include "shape.h"

class pixmap_formatter {
public:
  unsigned int image_width = 0;
  unsigned int image_height = 0;

  [[nodiscard]] std::string format_header() const {
    return std::format("P3\n{} {}\n{}", image_width, image_height,
                       max_color_level);
  }

  [[nodiscard]] std::string format_pixel(const color &pixel_color) const {
    auto color_channel_to_level = [](double ch) noexcept -> int {
      return int((double(max_color_level) + 0.999) * ch);
    };

    return std::format("{} {} {}", color_channel_to_level(pixel_color.r()),
                       color_channel_to_level(pixel_color.g()),
                       color_channel_to_level(pixel_color.b()));
  }

private:
  static constexpr int max_color_level = 255;
};

// Given a trace ray, interpolate a gradient.
inline color background_color(const ray_3 &trace_ray) noexcept {
  auto a = (norm(trace_ray.direction()).y() + 1.0) / 2.0;

  color c1(1.0, 1.0, 1.0), c2(0.5, 0.7, 1.0);

  color result = (1.0 - a) * c1 + a * c2;
  assert_color(result);

  return result;
}

// Given (u, v) coordinates of a viewport pixel, sample the world from the
// coordinates.
inline color trace_color(const pixmap_formatter &formatter,
                         const camera &main_camera,
                         const shape_range auto &world, double u,
                         double v) noexcept {
  static constexpr unsigned int max_trace_depth = 64;

  ray_3 trace_tail;
  {
    auto pixel_delta_u = main_camera.viewport_u() / formatter.image_width,
         pixel_delta_v = main_camera.viewport_v() / formatter.image_height;

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

    // We can use the iterative form without keeping a stack rather than
    // accumulate recursively because the accumulation is commutative.
    for (unsigned int trace_depth = 0; trace_depth < max_trace_depth;
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
          component_wise_product(trace_accumulation, attenuation);
    }
  }

  // Assume the final ray reaches the background (even though this may not
  // actually be the case), and multiply the background color. This will treat
  // the background color as the ambient light.
  color result =
      component_wise_product(trace_accumulation, background_color(trace_tail));
  assert_color(result);

  return result;
}

// Given the row and column of a formatter pixel, average samples of the world
// from the pixel.
inline color super_sampled_pixel_color(const pixmap_formatter &formatter,
                                       const camera &main_camera,
                                       const shape_range auto &world,
                                       unsigned int row,
                                       unsigned int column) noexcept {
  static constexpr unsigned int max_samples = 512;

  vector_3 sample_color_sum;
  for (unsigned int sample = 0; sample < max_samples; ++sample) {
    double u_jittered, v_jittered;
    {
      thread_local std::mt19937 generator{std::random_device{}()};
      thread_local std::uniform_real_distribution<double> distribution(0.0,
                                                                       1.0);
      u_jittered = column + distribution(generator) - 0.5,
      v_jittered = row + distribution(generator) - 0.5;
    }

    sample_color_sum +=
        trace_color(formatter, main_camera, world, u_jittered, v_jittered);
  }

  color result = sample_color_sum / max_samples;
  assert_color(result);

  return result;
}

void render(const pixmap_formatter &formatter, const camera &main_camera,
            const shape_range auto &world) {
  std::println("{}", formatter.format_header());

  for (unsigned int row = 0; row < formatter.image_height; ++row) {
    std::print(stderr, "\r\x1b[KRows of pixels remaining: {}/{}",
               formatter.image_height - row, formatter.image_height);

    for (unsigned int column = 0; column < formatter.image_width; ++column)
      std::println("{}", formatter.format_pixel(
                             linear_to_gamma_color(super_sampled_pixel_color(
                                 formatter, main_camera, world, row, column))));
  }
}
