#include <memory>
#include <numbers>
#include <vector>

#include "material.h"
#include "render.h"
#include "shape.h"

constexpr int image_width = 1280;
constexpr int image_height = 720;
constexpr double image_aspect_ratio = double(image_width) / image_height;

static const auto material_diffuse_light_green =
    std::make_shared<lambertian>(color(0.8, 0.8, 0.0));

static const auto material_diffuse_navy =
    std::make_shared<lambertian>(color(0.1, 0.2, 0.5));

static const auto material_dielectric_glass = std::make_shared<dielectric>(1.5);
static const auto material_dielectric_glass_inner =
    std::make_shared<dielectric>(1.0 / 1.5);

static const auto material_metal_greenish =
    std::make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

int main() {
  pixmap_formatter formatter;
  formatter.image_width = image_width;
  formatter.image_height = image_height;

  camera main_camera;
  main_camera.up = vector_3(0.0, 1.0, 0.0);
  main_camera.principal_ray =
      ray_3(point_3(-2.0, 2.0, 1.0), point_3(0.0, 0.0, -1.0));
  main_camera.vertical_field_of_view = std::numbers::pi / 9.0;
  main_camera.viewport_aspect_ratio = image_aspect_ratio;

  std::vector<shape> world{
      sphere(point_3(0.0, -100.5, -1.0), 100.0, material_diffuse_light_green),
      sphere(point_3(0.0, 0.0, -1.2), 0.5, material_diffuse_navy),
      sphere(point_3(-1.0, 0.0, -1.0), 0.5, material_dielectric_glass),
      sphere(point_3(-1.0, 0.0, -1.0), 0.4, material_dielectric_glass_inner),
      sphere(point_3(1.0, 0.0, -1.0), 0.5, material_metal_greenish)};

  render(formatter, main_camera, world);

  return 0;
}
