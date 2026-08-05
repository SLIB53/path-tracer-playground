#include <cstdio>
#include <print>

#include <color.h>

constexpr int image_width = 1024;
constexpr int image_height = 1024;

constexpr int p3_maxval = 255;

void write_header() {
  std::println("P3");
  std::println("{} {}", image_width, image_height);
  std::println("{}", p3_maxval);
}

void write_color(const color &pixel_color) {
  constexpr auto color_to_byte = [](const double ch) noexcept -> int {
    return int((double(p3_maxval) + 0.999) * ch);
  };

  std::println("{} {} {}", color_to_byte(pixel_color.r()),
               color_to_byte(pixel_color.g()), color_to_byte(pixel_color.b()));
}

void write_contents() {
  for (int row = 0; row < image_height; row++) {
    std::print(stderr, "\r\x1b[KLines remaining: {}/{}", image_height - row,
               image_height);

    for (int col = 0; col < image_width; col++) {
      write_color(color(double(col) / (image_width - 1),
                        double(row) / (image_height - 1), 0));
    }
  }
}

int main() {
  write_header();
  write_contents();

  std::println(stderr, "\r\x1b[KDone!");

  return 0;
}
