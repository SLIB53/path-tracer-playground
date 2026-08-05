#include <cstdio>
#include <print>

constexpr int image_width = 512;
constexpr int image_height = 512;

constexpr int p3_maxval = 255;

int main() {
  std::println("P3");
  std::println("{} {}", image_width, image_height);
  std::println("{}", p3_maxval);

  for (int row = 0; row < image_height; row++) {
    std::print(stderr, "\r\x1b[K{}/{} lines remaining", image_height - row,
               image_height);

    for (int col = 0; col < image_width; col++) {
      double redness = double(col) / (image_width - 1),
             greenness = double(row) / (image_height - 1);

      constexpr auto p3val_from_intensity =
          [](const double intensity) noexcept -> int {
        return int((double(p3_maxval) + 0.999) * intensity);
      };

      int r = p3val_from_intensity(redness),
          g = p3val_from_intensity(greenness), b = 0;

      std::println("{} {} {}", r, g, b);
    }
  }

  std::println(stderr, "\r\x1b[KDone!");

  return 0;
}
