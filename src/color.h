#pragma once

#include "interval.h"
#include "vector_3.h"

using color = vector_3;

[[nodiscard]] constexpr double
linear_to_gamma_color_channel(double ch) noexcept {
  return ch > 0 ? std::sqrt(ch) : 0;
};

constexpr color linear_to_gamma_color(const color &c) noexcept {
  return color(linear_to_gamma_color_channel(c.r()),
               linear_to_gamma_color_channel(c.g()),
               linear_to_gamma_color_channel(c.b()));
}

inline void assert_color_channel([[maybe_unused]] double ch) noexcept {
  assert(interval(0.0, 1.0).contains(ch));
}

inline void assert_color([[maybe_unused]] color c) noexcept {
  assert_color_channel(c.r());
  assert_color_channel(c.g());
  assert_color_channel(c.b());
}
