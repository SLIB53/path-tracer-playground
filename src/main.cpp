#include <cstdio>
#include <print>

#include "color.h"
#include "point_3.h"
#include "ray_3.h"
#include "spatial_vector_3.h"

constexpr int pixmap_max_level = 255;

constexpr int image_width = 1280;
constexpr int image_height = 720;

constexpr double camera_focal_length = 1.0;
constexpr point_3 camera_center;

constexpr double viewport_height = 2.0;
constexpr double viewport_width =
    viewport_height * (double(image_width) / image_height);

constexpr spatial_vector_3 viewport_u(viewport_width, 0.0, 0.0);
constexpr spatial_vector_3 viewport_v(0.0, -viewport_height, 0.0);
constexpr spatial_vector_3 viewport_origin =
    camera_center - spatial_vector_3(0.0, 0.0, camera_focal_length) -
    viewport_u / 2.0 - viewport_v / 2.0;

constexpr spatial_vector_3 pixel_delta_u = viewport_u / image_width;
constexpr spatial_vector_3 pixel_delta_v = viewport_v / image_height;
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
    // Given the camera ray, interpolate a gradient.
    static const auto background_color =
        [](const ray_3 &ray) noexcept -> color {
      auto a = (norm(ray.direction()).y() + 1.0) / 2.0;

      color c1(1.0, 1.0, 1.0);
      color c2(0.5, 0.7, 1.0);

      return (1.0 - a) * c1 + a * c2;
    };

    // Given the pixel ray that contacts a sphere with center sphere_center at
    // t, calculate the normal vector on the surface of the sphere, and map
    // the normal to a color.
    static const auto sphere_color = [](const ray_3 &ray, double t,
                                        const point_3 &sphere_center) -> color {
      spatial_vector_3 n = norm(ray.at(t) - sphere_center);

      return 0.5 * (color(n.x() + 1, n.y() + 1, n.z() + 1));
    };

    point_3 pixel_center =
        pixel_00_center + (col * pixel_delta_u) + (row * pixel_delta_v);

    ray_3 r(camera_center, pixel_center - camera_center);

    point_3 s_center(0.0, 0.0, -1.0);
    double s_radius = 0.5;

    auto t = intersects_sphere(r, s_center, s_radius);
    auto final_color =
        t > 0.0 ? sphere_color(r, t, s_center) : background_color(r);

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
