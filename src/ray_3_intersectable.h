#ifndef RAY_3_INTERSECTABLE
#define RAY_3_INTERSECTABLE

#include <variant>

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

#endif
