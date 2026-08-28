#pragma once

#include "ray_3.h"

class [[nodiscard]] camera {
public:
  vector_3 up;
  ray_3 principal_ray;
  double vertical_field_of_view = 0.0;
  double viewport_aspect_ratio = 0.0;

  inline vector_3 viewport_u() const noexcept {
    return viewport_width() * orthonormal_basis_u();
  }

  inline vector_3 viewport_v() const noexcept {
    return viewport_height() * -orthonormal_basis_v();
  }

  inline point_3 viewport_origin() const noexcept {
    return principal_ray.direction() - viewport_u() / 2.0 - viewport_v() / 2.0;
  }

private:
  [[nodiscard]] inline double viewport_height() const noexcept {
    return 2.0 * std::tan(vertical_field_of_view / 2.0) *
           principal_ray.length();
  }

  [[nodiscard]] inline double viewport_width() const noexcept {
    return viewport_height() * viewport_aspect_ratio;
  }

  inline vector_3 orthonormal_basis_w() const noexcept {
    return norm(principal_ray.origin() - principal_ray.direction());
  }

  inline vector_3 orthonormal_basis_u() const noexcept {
    return norm(cross(up, orthonormal_basis_w()));
  }

  inline vector_3 orthonormal_basis_v() const noexcept {
    return norm(cross(orthonormal_basis_w(), orthonormal_basis_u()));
  }
};
