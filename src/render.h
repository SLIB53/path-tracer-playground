#pragma once

#include <print>
#include <random>

#include "camera.h"
#include "shape.h"

class pixmap_formatter {
public:
  int image_width = 0;
  int image_height = 0;

  [[nodiscard]] std::string format_header() const {
    return std::format("P3\n{} {}\n{}", image_width, image_height,
                       color_level_max);
  }

  [[nodiscard]] std::string format_pixel(const color &pixel_color) const {
    auto color_channel_to_level = [](double ch) noexcept -> int {
      assert_color_channel(ch);

      return int((double(color_level_max) + 0.999) * ch);
    };

    return std::format("{} {} {}", color_channel_to_level(pixel_color.r()),
                       color_channel_to_level(pixel_color.g()),
                       color_channel_to_level(pixel_color.b()));
  }

private:
  static constexpr int color_level_max = 255;
};

void render(const pixmap_formatter &formatter, const camera &main_camera,
            const shape_range auto &world) {
  auto pixel_delta_u = main_camera.viewport_u() / formatter.image_width,
       pixel_delta_v = main_camera.viewport_v() / formatter.image_height;

  auto pixel_00_center =
      main_camera.viewport_origin() + (pixel_delta_u + pixel_delta_v) / 2.0;

  std::println("{}", formatter.format_header());

  for (int row = 0; row < formatter.image_height; ++row) {
    std::print(stderr, "\r\x1b[KRows of pixels remaining: {}/{}",
               formatter.image_height - row, formatter.image_height);

    for (int col = 0; col < formatter.image_width; ++col) {
      // Shade

      // Given the path ray, interpolate a gradient.
      static const auto background_color =
          [](const ray_3 &ray) noexcept -> color {
        auto a = (norm(ray.direction()).y() + 1.0) / 2.0;

        color c1(1.0, 1.0, 1.0), c2(0.5, 0.7, 1.0);

        auto result = (1.0 - a) * c1 + a * c2;
        assert_color(result);

        return result;
      };

      // Given a pixel's (u, v) coordinates, sample the world from the pixel.
      const auto pixel_sample_color = [&](double u,
                                          double v) noexcept -> color {
        auto pixel_sample_center =
            pixel_00_center + (u * pixel_delta_u) + (v * pixel_delta_v);

        color trace_accumulation =
            color(1.0, 1.0, 1.0); // HACK: initial value of (1, 1, 1) won't
                                  // contribute in pairwise multiplication.
        ray_3 trace_tail(main_camera.station_point,
                         pixel_sample_center - main_camera.station_point);
        {
          static constexpr unsigned int max_trace_depth = 16;
          for (unsigned int trace_depth = 0; trace_depth < max_trace_depth;
               ++trace_depth) {
            ray_3_intersection intersection;
            if (!intersects(world, trace_tail, interval(0.001, +infinity),
                            intersection))
              break;

            color attenuation;
            ray_3 scattered_ray;
            if (!intersection.surface_material->scatter(
                    trace_tail, intersection, attenuation, scattered_ray))
              break;

            assert(
                !approximately_equals(scattered_ray.direction(), vector_3()));

            trace_accumulation =
                pairwise_multiply(trace_accumulation, attenuation);

            trace_tail = scattered_ray;
          }
        }

        color result =
            pairwise_multiply(trace_accumulation, background_color(trace_tail));
        assert_color(result);

        return result;
      };

      color super_sampled_color;
      if (static constexpr int super_samples_per_pixel = 128;
          super_samples_per_pixel > 0) {
        for (int s = 0; s < super_samples_per_pixel; ++s) {
          thread_local std::mt19937 generator{std::random_device{}()};
          thread_local std::uniform_real_distribution<double> distribution(0.0,
                                                                           1.0);

          auto u_jittered = col + distribution(generator) - 0.5,
               v_jittered = row + distribution(generator) - 0.5;

          super_sampled_color += pixel_sample_color(u_jittered, v_jittered);
        }
        super_sampled_color /= super_samples_per_pixel;
      } else {
        auto u = col, v = row;

        super_sampled_color = pixel_sample_color(u, v);
      }
      assert_color(super_sampled_color);

      color final_color(linear_to_gamma_color(super_sampled_color));
      assert_color(final_color);

      // Output

      std::println("{}", formatter.format_pixel(final_color));
    }
  }
}
