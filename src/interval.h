#pragma once

#include <limits>

constexpr auto infinity = std::numeric_limits<double>::infinity();

class [[nodiscard]] interval {
public:
  double min, max;

  constexpr interval() noexcept : min(+infinity), max(-infinity) {}

  constexpr interval(double min, double max) noexcept : min(min), max(max) {}

  [[nodiscard]] constexpr bool contains(double x) const noexcept {
    return x >= min && x <= max;
  }

  [[nodiscard]] constexpr bool surrounds(double x) const noexcept {
    return x > min && x < max;
  }
};
