#pragma once

#include "ray_3.h"

class [[nodiscard]] camera {
public:
  vector_3 up;
  ray_3 axial_ray;
  double field_of_view_vertical_angle = 0.0;
  double depth_of_field_angle = 0.0;
  double viewport_distance = 0.0;
  double viewport_aspect_ratio = 0.0;

  vector_3 viewport_u() const noexcept {
    return viewport_width() * orthonormal_basis_u();
  }

  vector_3 viewport_v() const noexcept {
    return viewport_height() * -orthonormal_basis_v();
  }

  point_3 viewport_origin() const noexcept {
    return axial_ray.origin() - viewport_distance * orthonormal_basis_w() -
           viewport_u() / 2.0 - viewport_v() / 2.0;
  }

  vector_3 depth_of_field_disk_u() const noexcept {
    return depth_of_field_disk_radius() * orthonormal_basis_u();
  }

  vector_3 depth_of_field_disk_v() const noexcept {
    return depth_of_field_disk_radius() * orthonormal_basis_v();
  }

private:
  [[nodiscard]] double viewport_width() const noexcept {
    return viewport_height() * viewport_aspect_ratio;
  }

  [[nodiscard]] double viewport_height() const noexcept {
    return 2.0 * viewport_distance *
           std::tan(field_of_view_vertical_angle / 2.0);
  }

  vector_3 orthonormal_basis_u() const noexcept {
    return normalize(cross(up, orthonormal_basis_w()));
  }

  vector_3 orthonormal_basis_v() const noexcept {
    return normalize(cross(orthonormal_basis_w(), orthonormal_basis_u()));
  }

  vector_3 orthonormal_basis_w() const noexcept {
    return normalize(axial_ray.origin() - axial_ray.direction());
  }

  [[nodiscard]] double depth_of_field_disk_radius() const noexcept {
    return viewport_distance * std::tan(depth_of_field_angle / 2.0);
  }
};
