#include <algorithm>
#include <numbers>

#include "pixmap_formatter.h"
#include "ray_tracer.h"

int main() {
  constexpr unsigned image_width = 5120;
  constexpr unsigned image_height = 2880;

  pixmap_formatter formatter;
  formatter.image_width = image_width;
  formatter.image_height = image_height;

  ray_tracer engine;
  engine.image_width = image_width;
  engine.image_height = image_height;
  engine.samples_per_pixel = 512;
  engine.trace_depth_maximum = 64;

  camera main_camera;
  main_camera.up = vector_3(0.0, 1.0, 0.0);
  main_camera.axial_ray = ray_3(point_3(13.0, 2.0, 3.0), vector_3());
  main_camera.field_of_view_vertical_angle = std::numbers::pi / 9.0;
  main_camera.depth_of_field_angle = std::numbers::pi / 180.0;
  main_camera.viewport_distance = 10.0;
  main_camera.viewport_aspect_ratio = double(image_width) / image_height;

  std::vector<shape> world;
  {
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

    const auto spawn_orb_hollow_diamond =
        [&world](const point_3 &center) -> void {
      constexpr auto refraction_index_diamond = 2.417;

      auto material_outer =
               std::make_shared<dielectric>(refraction_index_diamond),
           material_inner =
               std::make_shared<dielectric>(1.0 / refraction_index_diamond);
      world.push_back(sphere(center, std::fabs(center.y()), material_outer));
      world.push_back(
          sphere(center, 0.854 * std::fabs(center.y()), material_inner));
    };

    // spawn big orbs

    spawn_orb_lambertian(point_3(-4.0, 1.0, 0.0), base16_palette[15]);
    spawn_orb_metal(point_3(0.0, 1.0, 0.0), base16_palette[1], 0.0);
    spawn_orb_hollow_diamond(point_3(4.0, 1.0, 0.0));

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
        auto candidate_radius = random_in_interval(half_to_one) * 0.333;
        point_3 candidate_center(a + 0.999 * random_in_interval(zero_to_one),
                                 candidate_radius,
                                 b + 0.999 * random_in_interval(zero_to_one));
        if (orb_overlaps_existing(candidate_center, candidate_radius))
          continue;

        if (auto choice = random_in_interval(zero_to_one); choice < 0.618)
          spawn_orb_lambertian(candidate_center, random_base16_normal_color());
        else if (choice < 0.854)
          spawn_orb_metal(candidate_center, random_base16_bright_color(),
                          random_in_interval(zero_to_one));
        else
          spawn_orb_hollow_diamond(candidate_center);
      }

    // spawn ground

    spawn_orb_lambertian(point_3(0.0, -1000.0, 0.0), base16_palette[0]);
  }

  std::vector<color> imagebuffer;
  engine.render(main_camera, world, imagebuffer);

  formatter.print(imagebuffer);

  return 0;
}
