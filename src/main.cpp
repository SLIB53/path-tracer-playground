#include <cstdio>
#include <print>

#include <color.h>

constexpr int image_width = 1024;
constexpr int image_height = 1024;

constexpr int pixmap_max_level = 255;

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
  constexpr auto color_channel_to_level = [](double ch) noexcept -> int {
    return int((double(pixmap_max_level) + 0.999) * ch);
  };

  std::println("{} {} {}", color_channel_to_level(pixel_color.r()),
               color_channel_to_level(pixel_color.g()),
               color_channel_to_level(pixel_color.b()));
}

template <class F> void write_pixels(F &&each_row) {
  for_each_pixel(image_width, image_height, each_row, [](int row, int col) {
    write_pixel(color(double(col) / (image_width - 1),
                      double(row) / (image_height - 1), 0));
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
