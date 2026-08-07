#include <cstdio>
#include <print>
#include <vector>

#include "color.h"
#include "shape.h"

constexpr int pixmap_max_level = 255;

constexpr int image_width = 1280;
constexpr int image_height = 720;

constexpr double camera_focal_length = 1.0;
constexpr point_3 camera_center;

constexpr double viewport_height = 2.0;
constexpr double viewport_width =
    viewport_height * (double(image_width) / image_height);

constexpr vector_3 viewport_u(viewport_width, 0.0, 0.0);
constexpr vector_3 viewport_v(0.0, -viewport_height, 0.0);
constexpr vector_3 viewport_origin = camera_center -
                                     vector_3(0.0, 0.0, camera_focal_length) -
                                     viewport_u / 2.0 - viewport_v / 2.0;

constexpr vector_3 pixel_delta_u = viewport_u / image_width;
constexpr vector_3 pixel_delta_v = viewport_v / image_height;
constexpr point_3 pixel_00_center =
    viewport_origin + (pixel_delta_u + pixel_delta_v) / 2.0;

template <class F1, class F2>
void for_each_pixel(int width, int height, F1 &&each_row, F2 &&each_pixel) {
  for (int row = 0; row < height; ++row) {
    each_row(row);

    for (int col = 0; col < width; ++col) {
      each_pixel(row, col);
    }
  }
}

void write_header() {
  std::println("P3\n{} {}\n{}", image_width, image_height, pixmap_max_level);
}

void write_pixel(const color &pixel_color) {
  static auto color_channel_to_level = [](double ch) noexcept -> int {
    return int((double(pixmap_max_level) + 0.999) * ch);
  };

  std::println("{} {} {}", color_channel_to_level(pixel_color.r()),
               color_channel_to_level(pixel_color.g()),
               color_channel_to_level(pixel_color.b()));
}

template <class F> void write_pixels(F &&each_row) {
  for_each_pixel(image_width, image_height, each_row, [](int row, int col) {
    // Shade

    // Given the camera ray, interpolate a gradient.
    static const auto background_color =
        [](const ray_3 &ray) noexcept -> color {
      auto a = (norm(ray.direction()).y() + 1.0) / 2.0;

      color c1(1.0, 1.0, 1.0);
      color c2(0.5, 0.7, 1.0);

      return (1.0 - a) * c1 + a * c2;
    };

    // Given the intersection where the pixel ray contacts the sphere, use the
    // normal vector at the contact point on the surface of the sphere to map to
    // a color.
    static const auto sphere_color =
        [](ray_3_intersection intersection) -> color {
      return 0.5 *
             (color(intersection.normal.x() + 1, intersection.normal.y() + 1,
                    intersection.normal.z() + 1));
    };

    point_3 pixel_center =
        pixel_00_center + (col * pixel_delta_u) + (row * pixel_delta_v);

    ray_3 r(camera_center, pixel_center - camera_center);

    std::vector<shape> world{sphere(point_3(0.0, 0.0, -1.0), 0.5),
                             sphere(point_3(0, -100.5, -1), 100)};

    ray_3_intersection rwi;
    auto final_color = intersects(world, r, 0.0, 1000.0, rwi)
                           ? sphere_color(rwi)
                           : background_color(r);

    // Write

    write_pixel(final_color);
  });
}

int main() {
  write_header();

  write_pixels([](int row) {
    std::print(stderr, "\r\x1b[KRows of pixels remaining: {}/{}",
               image_height - row, image_height);
  });

  std::println(stderr, "\r\x1b[KDone!");

  return 0;
}
