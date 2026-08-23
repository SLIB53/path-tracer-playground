#pragma once

#include <memory>

#include "point_3.h"
#include "ray_3.h"

class material;

class ray_3_intersection {
public:
  double t = 0.0;
  point_3 point;
  vector_3 normal;
  std::shared_ptr<material> surface_material;
  bool from_front = false;
};

constexpr bool
intersects_from_front(const ray_3 &ray, const vector_3 &front_normal,
                      vector_3 &out_intersection_normal) noexcept {
  auto from_front = dot(ray.direction(), front_normal) < 0.0;

  out_intersection_normal = from_front ? front_normal : -front_normal;

  return from_front;
}
