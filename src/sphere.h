#pragma once

#include "ray_3_intersection.h"

class sphere {
public:
  constexpr sphere() noexcept = default;

  constexpr sphere(const point_3 &center, double radius) noexcept
      : center_(center), radius_(radius) {}

  constexpr bool intersects(const ray_3 &ray, double ray_t_min,
                            double ray_t_max,
                            ray_3_intersection &out) const noexcept {
    auto ray_origin_toward_center = center_ - ray.origin();

    auto a = ray.direction().length_squared();
    auto h = dot(ray.direction(), ray_origin_toward_center);
    auto c = ray_origin_toward_center.length_squared() - radius_ * radius_;

    auto discriminant = h * h - a * c;

    if (discriminant < 0.0)
      return false;

    auto discriminant_sqrt = std::sqrt(discriminant);

    auto is_outside_t_range = [&ray_t_min, &ray_t_max](double d) -> bool {
      return d < ray_t_min || d > ray_t_max;
    };
    double root_nearest;
    if (root_nearest = (h - discriminant_sqrt) / a;
        is_outside_t_range(root_nearest))
      if (root_nearest = (h + discriminant_sqrt) / a;
          is_outside_t_range(root_nearest))
        return false;

    out.t = root_nearest;
    out.point = ray.at(out.t);
    out.from_front =
        intersects_from_front(ray, (out.point - center_) / radius_, out.normal);

    return true;
  }

private:
  point_3 center_;
  double radius_;
};
