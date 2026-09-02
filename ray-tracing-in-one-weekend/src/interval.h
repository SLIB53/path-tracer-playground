#pragma once

#include <limits>
#include <random>

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

[[nodiscard]] inline double random_in_interval(const interval intrvl) {
  thread_local std::mt19937 generator{std::random_device{}()};
  std::uniform_real_distribution<double> distribution(intrvl.min, intrvl.max);

  return distribution(generator);
}

constexpr interval negative_one_to_one(-1.0, 1.0);
constexpr interval zero_to_one(0.0, 1.0);
constexpr interval half_to_one(0.5, 1.0);