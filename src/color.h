#pragma once

#include "interval.h"
#include "vector_3.h"

using color = vector_3;

[[nodiscard]] inline double
linear_to_gamma_color_channel(double channel) noexcept {
  return channel > 0 ? std::sqrt(channel) : 0;
};

inline color linear_to_gamma_color(const color &clr) noexcept {
  return color(linear_to_gamma_color_channel(clr.r()),
               linear_to_gamma_color_channel(clr.g()),
               linear_to_gamma_color_channel(clr.b()));
}

inline void assert_color_channel([[maybe_unused]] double channel) noexcept {
  assert(interval(0.0, 1.0).contains(channel));
}

inline void assert_color([[maybe_unused]] const color &clr) noexcept {
  assert_color_channel(clr.r());
  assert_color_channel(clr.g());
  assert_color_channel(clr.b());
}
