#include <algorithm>
#include <numbers>

#include "pixmap_formatter.h"
#include "render.h"

int main() {
  constexpr unsigned image_width = 5120;
  constexpr unsigned image_height = 2880;

  pixmap_formatter formatter;
  formatter.image_width = image_width;
  formatter.image_height = image_height;

  camera main_camera;
  main_camera.up = vector_3(0.0, 1.0, 0.0);
  main_camera.axial_ray = ray_3(point_3(13.0, 2.0, 3.0), vector_3());
  main_camera.field_of_view_vertical_angle = std::numbers::pi / 9.0;
  main_camera.depth_of_field_angle = std::numbers::pi / 180.0;
  main_camera.viewport_distance = 10.0;
  main_camera.viewport_aspect_ratio = double(image_width) / image_height;

  std::vector<shape> world;
  {
    std::mt19937 generator{std::random_device{}()};
    std::uniform_real_distribution<double> zero_to_one(0.0, 1.0);
    std::uniform_real_distribution<double> half_to_one(0.5, 1.0);

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

    const auto spawn_orb_dielectric =
        [&world](const point_3 &center, double refraction_index) -> void {
      auto material_outer = std::make_shared<dielectric>(refraction_index),
           material_inner =
               std::make_shared<dielectric>(1.0 / refraction_index);
      world.push_back(sphere(center, std::fabs(center.y()), material_outer));
      world.push_back(
          sphere(center, 0.854 * std::fabs(center.y()), material_inner));
    };

    // spawn big orbs

    spawn_orb_lambertian(point_3(-4.0, 1.0, 0.0), palette::at(15));
    spawn_orb_dielectric(point_3(4.0, 1.0, 0.0), 2.417); // diamond
    spawn_orb_metal(point_3(0.0, 1.0, 0.0), palette::at(1), 0.0);

    // spawn small orbs

    const auto orb_overlaps_existing =
        [&world](const point_3 &candidate_center,
                 double candidate_radius) -> bool {
      return std::ranges::any_of(world, [&](const auto &existing) -> bool {
        const sphere *existing_sphere = std::get_if<sphere>(&existing);

        // WARNING: Skipping null check on existing_sphere. This assumes we will
        // only have spheres.

        return (candidate_center - existing_sphere->center()).length() <
               (candidate_radius + existing_sphere->radius());
      });
    };

    for (int a = -12; a < 12; ++a)
      for (int b = -12; b < 12; ++b) {
        auto candidate_radius = half_to_one(generator) * 0.333;
        point_3 candidate_center(a + 0.999 * zero_to_one(generator),
                                 candidate_radius,
                                 b + 0.999 * zero_to_one(generator));
        if (orb_overlaps_existing(candidate_center, candidate_radius))
          continue;

        if (auto choice = zero_to_one(generator); choice < 0.618)
          spawn_orb_lambertian(candidate_center, palette::random_standard());
        else if (choice < 0.944)
          spawn_orb_metal(candidate_center, palette::random_bright(),
                          zero_to_one(generator));
        else
          spawn_orb_dielectric(candidate_center, 2.417); // diamond
      }

    // spawn ground

    spawn_orb_lambertian(point_3(0.0, -1000.0, 0.0), palette::at(0));
  }

  std::vector<color> imagebuffer;
  render(image_width, image_height, main_camera, world, imagebuffer);

  formatter.print(imagebuffer);

  return 0;
}
