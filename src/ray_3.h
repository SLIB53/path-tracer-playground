#ifndef RAY_3
#define RAY_3

#include <concepts>

#include "point_3.h"
#include "vector_3.h"

class ray_3 {
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

  [[nodiscard]] constexpr point_3 at(double t) const noexcept {
    return origin_ + t * direction_;
  }

private:
  point_3 origin_;
  vector_3 direction_;
};

class ray_3_intersection {
public:
  double t;
  point_3 contact_point;
  vector_3 normal;
};

template <class T>
concept ray_3_intersectable =
    requires(const T &shape, const ray_3 &ray, double ray_t_min,
             double ray_t_max, ray_3_intersection &out) {
      {
        shape.intersects(ray, ray_t_min, ray_t_max, out)
      } -> std::same_as<bool>;
    };

#endif
