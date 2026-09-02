#pragma once

#include <print>

#include "color.h"

class pixmap_formatter {
public:
  unsigned image_width = 0;
  unsigned image_height = 0;

  void print(const std::vector<color> &imagebuffer) const {
    std::println("{}", format_header());

    for (std::size_t i = 0; i < imagebuffer.size(); ++i) {
      std::print(stderr, "\r\x1b[K{:.0f}% ({}/{} pixels)",
                 double(i) / imagebuffer.size() * 100.0, i, imagebuffer.size());

      std::println("{}", format_pixel(imagebuffer[i]));
    }

    std::println(stderr, "\r\x1b[K{:.0f}% ({}/{} pixels)", 100.0,
                 imagebuffer.size(), imagebuffer.size());
  }

private:
  static constexpr unsigned max_color_level = 255;

  [[nodiscard]] std::string format_header() const {
    return std::format("P3\n{} {}\n{}", image_width, image_height,
                       max_color_level);
  }

  [[nodiscard]] std::string format_pixel(const color &pixel_color) const {
    const auto color_channel_to_color_level =
        [](double channel) noexcept -> int {
      return int((double(max_color_level) + 0.999) * channel);
    };

    return std::format("{} {} {}",
                       color_channel_to_color_level(pixel_color.r()),
                       color_channel_to_color_level(pixel_color.g()),
                       color_channel_to_color_level(pixel_color.b()));
  }
};
