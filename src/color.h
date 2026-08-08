#pragma once

#include "interval.h"
#include "vector_3.h"

using color = vector_3;

inline void assert_color_channel(double ch) noexcept {
  assert(interval(0.0, 1.0).contains(ch));
}

inline void assert_color(color c) noexcept {
  assert_color_channel(c.r());
  assert_color_channel(c.g());
  assert_color_channel(c.b());
}
