#pragma once

#include "point_3.h"

class [[nodiscard]] ray_3 {
public:
  constexpr ray_3() noexcept = default;

  constexpr ray_3(const point_3 &origin, const vector_3 &direction) noexcept
      : origin_(origin), direction_(direction) {}

  [[nodiscard]] constexpr const point_3 &origin() const noexcept {
    return origin_;
  }

  [[nodiscard]] constexpr const vector_3 &direction() const noexcept {
    return direction_;
  }

  constexpr point_3 at(double t) const noexcept {
    return origin_ + t * direction_;
  }

private:
  point_3 origin_;
  vector_3 direction_;
};
