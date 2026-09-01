#pragma once

#include <print>

#include "color.h"

class pixmap_formatter {
public:
  unsigned image_width = 0;
  unsigned image_height = 0;

  void print(const std::vector<color> &imagebuffer) const {
    std::println("{}", format_header());

    for (const auto &c : imagebuffer)
      std::println("{}", format_pixel(c));
  }

private:
  static constexpr unsigned max_color_level = 255;

  [[nodiscard]] std::string format_header() const {
    return std::format("P3\n{} {}\n{}", image_width, image_height,
                       max_color_level);
  }

  [[nodiscard]] std::string format_pixel(const color &pixel_color) const {
    auto color_channel_to_level = [](double channel) noexcept -> int {
      return int((double(max_color_level) + 0.999) * channel);
    };

    return std::format("{} {} {}", color_channel_to_level(pixel_color.r()),
                       color_channel_to_level(pixel_color.g()),
                       color_channel_to_level(pixel_color.b()));
  }
};
