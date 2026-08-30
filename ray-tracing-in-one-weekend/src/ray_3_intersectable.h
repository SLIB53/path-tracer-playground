#pragma once

#include <variant>

#include "sphere.h"

template <class T>
concept ray_3_intersectable =
    requires(const T &shape, const ray_3 &ray, interval ray_t,
             ray_3_intersection &out_intersection) {
      { shape.intersects(ray, ray_t, out_intersection) } -> std::same_as<bool>;
    };

using any_ray_3_intersectable = std::variant<sphere>;

template <class T>
concept any_ray_3_intersectable_range =
    std::ranges::forward_range<T> &&
    std::same_as<std::ranges::range_value_t<T>, any_ray_3_intersectable>;

constexpr bool
intersects(const any_ray_3_intersectable_range auto &intersectables,
           const ray_3 &ray, interval ray_t,
           ray_3_intersection &out_intersection) noexcept {
  auto intersection = out_intersection;
  auto has_intersected = false;
  auto nearest_t = ray_t.max;

  for (const auto &intersectable : intersectables) {
    std::visit(
        [&](const ray_3_intersectable auto &intersectable_visiting) {
          if (intersectable_visiting.intersects(
                  ray, interval(ray_t.min, nearest_t), intersection)) {
            has_intersected = true;
            nearest_t = intersection.t;
          }
        },
        intersectable);
  }

  out_intersection = intersection;

  return has_intersected;
}
