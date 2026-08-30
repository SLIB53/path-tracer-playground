#pragma once

#include "material.h"

class [[nodiscard]] sphere {
public:
  constexpr sphere() noexcept = default;

  constexpr sphere(const point_3 &center, double radius,
                   std::shared_ptr<material> surface_material) noexcept
      : center_(center), radius_(radius), surface_material_(surface_material) {}

  bool intersects(const ray_3 &ray, interval ray_t,
                  ray_3_intersection &out_intersection) const noexcept {
    auto ray_origin_toward_center = center_ - ray.origin();

    auto a = ray.direction().length_squared();
    auto h = dot(ray.direction(), ray_origin_toward_center);
    auto c = ray_origin_toward_center.length_squared() - radius_ * radius_;

    auto discriminant = h * h - a * c;

    if (discriminant < 0.0)
      return false;

    auto discriminant_sqrt = std::sqrt(discriminant);

    double root_nearest;
    if (root_nearest = (h - discriminant_sqrt) / a;
        !ray_t.surrounds(root_nearest))
      if (root_nearest = (h + discriminant_sqrt) / a;
          !ray_t.surrounds(root_nearest))
        return false;

    out_intersection.t = root_nearest;
    out_intersection.point = ray.at(out_intersection.t);
    out_intersection.surface_material = surface_material_;
    out_intersection.from_front =
        intersects_from_front(ray, (out_intersection.point - center_) / radius_,
                              out_intersection.normal);

    return true;
  }

private:
  point_3 center_;
  double radius_ = 0.0;
  std::shared_ptr<material> surface_material_;
};
