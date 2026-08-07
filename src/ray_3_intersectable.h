#ifndef RAY_3_INTERSECTABLE
#define RAY_3_INTERSECTABLE

#include <ranges>
#include <variant>

#include "ray_3_intersection.h"
#include "sphere.h"

template <class T>
concept ray_3_intersectable =
    requires(const T &shape, const ray_3 &ray, double ray_t_min,
             double ray_t_max, ray_3_intersection &out) {
      {
        shape.intersects(ray, ray_t_min, ray_t_max, out)
      } -> std::same_as<bool>;
    };

using any_ray_3_intersectable = std::variant<sphere>;

template <class T>
concept any_ray_3_intersectable_range =
    std::ranges::forward_range<T> &&
    std::same_as<std::ranges::range_value_t<T>, any_ray_3_intersectable>;

constexpr bool
intersects(const any_ray_3_intersectable_range auto &intersectables,
           const ray_3 &ray, double ray_t_min, double ray_t_max,
           ray_3_intersection &out) noexcept {
  auto rai = out;
  auto has_intersected = false;
  auto nearest_t = ray_t_max;

  for (const auto &i : intersectables) {
    std::visit(
        [&](const auto &ic) {
          if (ic.intersects(ray, ray_t_min, nearest_t, rai)) {
            has_intersected = true;
            nearest_t = rai.t;
          }
        },
        i);
  }

  out = rai;

  return has_intersected;
}

#endif
