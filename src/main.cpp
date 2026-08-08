#include <vector>

#include "render.h"
#include "shape.h"

constexpr int image_width = 1280;
constexpr int image_height = 720;
constexpr double image_aspect_ratio = double(image_width) / image_height;

int main() {
  pixmap_formatter formatter;
  formatter.image_width = image_width;
  formatter.image_height = image_height;

  camera main_camera;
  main_camera.focal_length = 1.0;
  main_camera.viewport_height = 2.0;
  main_camera.viewport_width = main_camera.viewport_height * image_aspect_ratio;

  std::vector<shape> world{sphere(point_3(0.0, 0.0, -1.0), 0.5),
                           sphere(point_3(0, -100.5, -1), 100)};

  render(formatter, main_camera, world);

  return 0;
}
