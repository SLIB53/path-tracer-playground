#include <numbers>

#include "render.h"

constexpr int image_width = 1280 / 4;
constexpr int image_height = 720 / 4;
constexpr double image_aspect_ratio = double(image_width) / image_height;

int main() {
  pixmap_formatter formatter;
  formatter.image_width = image_width;
  formatter.image_height = image_height;

  camera main_camera;
  main_camera.up = vector_3(0.0, 1.0, 0.0);
  main_camera.axial_ray = ray_3(point_3(13.0, 2.0, 3.0), vector_3());
  main_camera.field_of_view_vertical_angle = std::numbers::pi / 9.0;
  main_camera.depth_of_field_angle = std::numbers::pi / 300.0;
  main_camera.viewport_distance = 10.0;
  main_camera.viewport_aspect_ratio = image_aspect_ratio;

  std::vector<shape> world;
  {
    thread_local std::mt19937 generator{std::random_device{}()};
    thread_local std::uniform_real_distribution<double> zero_to_one(0.0, 1.0);
    thread_local std::uniform_real_distribution<double> half_to_one(0.5, 1.0);

    const auto spawn_orb_lambertian = [&world](const point_3 &center,
                                               const color &albedo) -> void {
      auto material = std::make_shared<lambertian>(albedo);
      world.push_back(sphere(center, std::fabs(center.y()), material));
    };

    const auto spawn_orb_metal = [&world](const point_3 &center,
                                          const color &albedo,
                                          double fuzz) -> void {
      auto material = std::make_shared<metal>(albedo, fuzz);
      world.push_back(sphere(center, std::fabs(center.y()), material));
    };

    const auto spawn_orb_glass = [&world](const point_3 &center) -> void {
      auto material = std::make_shared<dielectric>(1.5);
      world.push_back(sphere(center, std::fabs(center.y()), material));
    };

    // spawn ground

    spawn_orb_lambertian(point_3(0.0, -1000.0, 0.0), color(0.5, 0.5, 0.5));

    // spawn small orbs

    for (int a = -11; a < 11; ++a)
      for (int b = -11; b < 11; ++b) {
        point_3 center(a + 0.9 * zero_to_one(generator), 0.2,
                       b + 0.9 * zero_to_one(generator));
        if ((center - point_3(4, center.y(), 0)).length() <= 0.9)
          break;

        if (auto choice = zero_to_one(generator); choice < 0.8)
          spawn_orb_lambertian(
              center, component_wise_product(color::random(), color::random()));
        else if (choice < 0.95)
          spawn_orb_metal(center, color::random(0.5, 1),
                          half_to_one(generator));
        else
          spawn_orb_glass(center);
      }

    // spawn big orbs

    spawn_orb_lambertian(point_3(-4, 1, 0), color(0.4, 0.2, 0.1));

    spawn_orb_glass(point_3(0, 1, 0));

    spawn_orb_metal(point_3(4, 1, 0), color(0.7, 0.6, 0.5), 0.0);
  }

  render(formatter, main_camera, world);

  return 0;
}
