#ifndef RAY_3
#define RAY_3

#include "point_3.h"

class ray_3 {
public:
  constexpr ray_3() noexcept = default;

  constexpr ray_3(const point_3 &origin,
                  const spatial_vector_3 &direction) noexcept
      : origin_(origin), direction_(direction) {}

  [[nodiscard]] constexpr const point_3 &origin() const noexcept {
    return origin_;
  }

  [[nodiscard]] constexpr const spatial_vector_3 &direction() const noexcept {
    return direction_;
  }

  [[nodiscard]] constexpr point_3 at(double t) const noexcept {
    return origin_ + t * direction_;
  }

private:
  point_3 origin_;
  spatial_vector_3 direction_;
};

[[nodiscard]] constexpr double
intersects_sphere(const ray_3 &ray, const point_3 &sphere_center,
                  double sphere_radius) noexcept {
  auto c_minus_q = sphere_center - ray.origin();

  auto a = ray.direction().length_squared();
  auto h = dot(ray.direction(), c_minus_q);
  auto c = c_minus_q.length_squared() - sphere_radius * sphere_radius;

  auto discriminant = h * h - a * c;

  return discriminant >= 0.0 ? (h - std::sqrt(discriminant)) / a : -1.0;
}

#endif
