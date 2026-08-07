#pragma once

#include <print>

#include "camera.h"
#include "color.h"
#include "shape.h"

class pixmap_formatter {
public:
  int image_width = 0;
  int image_height = 0;
  int color_level_max = 255;

  [[nodiscard]] std::string format_header() const {
    return std::format("P3\n{} {}\n{}", image_width, image_height,
                       color_level_max);
  }

  [[nodiscard]] std::string format_pixel(const color &pixel_color) const {
    auto color_channel_to_level = [this](double ch) noexcept -> int {
      return int((double(color_level_max) + 0.999) * ch);
    };

    return std::format("{} {} {}", color_channel_to_level(pixel_color.r()),
                       color_channel_to_level(pixel_color.g()),
                       color_channel_to_level(pixel_color.b()));
  }
};

void render(const pixmap_formatter &formatter, const camera &main_camera,
            const shape_range auto &world) {
  auto pixel_delta_u = main_camera.viewport_u() / formatter.image_width;
  auto pixel_delta_v = main_camera.viewport_v() / formatter.image_height;
  auto pixel_00_center =
      main_camera.viewport_origin() + (pixel_delta_u + pixel_delta_v) / 2.0;

  std::println("{}", formatter.format_header());

  for (int row = 0; row < formatter.image_height; ++row) {
    std::print(stderr, "\r\x1b[KRows of pixels remaining: {}/{}",
               formatter.image_height - row, formatter.image_height);

    for (int col = 0; col < formatter.image_width; ++col) {
      // Shade

      // Given the camera ray, interpolate a gradient.
      static const auto background_color =
          [](const ray_3 &ray) noexcept -> color {
        auto a = (norm(ray.direction()).y() + 1.0) / 2.0;

        color c1(1.0, 1.0, 1.0);
        color c2(0.5, 0.7, 1.0);

        return (1.0 - a) * c1 + a * c2;
      };

      // Given the intersection where the camera ray contacts a shape, use the
      // normal vector at the contact point on the surface of the sphere to map
      // to a color.
      static const auto shape_color =
          [](ray_3_intersection intersection) noexcept -> color {
        return 0.5 *
               (color(intersection.normal.x() + 1, intersection.normal.y() + 1,
                      intersection.normal.z() + 1));
      };

      point_3 pixel_center =
          pixel_00_center + (col * pixel_delta_u) + (row * pixel_delta_v);

      ray_3 camera_ray(main_camera.station_point,
                       pixel_center - main_camera.station_point);

      ray_3_intersection intersection;
      auto final_color =
          intersects(world, camera_ray, gte_zero_interval, intersection)
              ? shape_color(intersection)
              : background_color(camera_ray);

      // Output

      std::println("{}", formatter.format_pixel(final_color));
    }
  }
}
