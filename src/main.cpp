#include <numbers>

#include "render.h"

constexpr int image_width = 1280;
constexpr int image_height = 720;
constexpr double image_aspect_ratio = double(image_width) / image_height;

int main() {
  pixmap_formatter formatter;
  formatter.image_width = image_width;
  formatter.image_height = image_height;

  camera main_camera;
  main_camera.up = vector_3(0.0, 1.0, 0.0);
  main_camera.principal_ray = ray_3(point_3(13.0, 2.0, 3.0), vector_3());
  main_camera.field_of_view_vertical_angle = std::numbers::pi / 9.0;
  main_camera.depth_of_field_angle = std::numbers::pi / 300.0;
  main_camera.viewport_distance = 10.0;
  main_camera.viewport_aspect_ratio = image_aspect_ratio;

  std::vector<shape> world;
  {
    // spawn ground

    auto ground_material = std::make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.push_back(
        sphere(point_3(0.0, -1000.0, 0.0), 1000.0, ground_material));

    // spawn small orbs

    for (int a = -11; a < 11; ++a)
      for (int b = -11; b < 11; ++b) {
        thread_local std::mt19937 generator{std::random_device{}()};
        thread_local std::uniform_real_distribution<double> zero_to_one(0.0,
                                                                        1.0);

        static const auto spawn_orb_lambertian =
            [&world](const point_3 &center) -> void {
          auto albedo = pairwise_multiply(color::random(), color::random());
          auto orb_material = std::make_shared<lambertian>(albedo);
          world.push_back(sphere(center, center.y(), orb_material));
        };

        static const auto spawn_orb_metal =
            [&world](const point_3 &center) -> void {
          thread_local std::uniform_real_distribution<double> zero_to_half(0.0,
                                                                           1.0);
          auto albedo = color::random(0.5, 1);
          auto fuzz = zero_to_half(generator);
          auto orb_material = std::make_shared<metal>(albedo, fuzz);
          world.push_back(sphere(center, center.y(), orb_material));
        };

        static const auto spawn_orb_glass =
            [&world](const point_3 &center) -> void {
          auto orb_material = std::make_shared<dielectric>(1.5);
          world.push_back(sphere(center, center.y(), orb_material));
        };

        point_3 center(a + 0.9 * zero_to_one(generator), 0.2,
                       b + 0.9 * zero_to_one(generator));
        if ((center - point_3(4, center.y(), 0)).length() <= 0.9)
          break;

        if (auto raffle = zero_to_one(generator); raffle < 0.8)
          spawn_orb_lambertian(center);
        else if (raffle < 0.95)
          spawn_orb_metal(center);
        else
          spawn_orb_glass(center);
      }

    // spawn big orbs

    auto center_orb_material = std::make_shared<dielectric>(1.5);
    world.push_back(sphere(point_3(0, 1, 0), 1.0, center_orb_material));

    auto far_orb_material = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.push_back(sphere(point_3(-4, 1, 0), 1.0, far_orb_material));

    auto near_orb_material = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.push_back(sphere(point_3(4, 1, 0), 1.0, near_orb_material));
  }

  render(formatter, main_camera, world);

  return 0;
}
