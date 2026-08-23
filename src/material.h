#pragma once

#include "color.h"
#include "ray_3.h"
#include "ray_3_intersection.h"

class material {
public:
  virtual ~material() = default;

  virtual bool scatter(const ray_3 &incoming_ray,
                       const ray_3_intersection &intersection,
                       color &out_attenuation, ray_3 &out_scattered_ray) const {
    return false;
  }
};

class lambertian : public material {
public:
  lambertian(const color &albedo) : albedo_(albedo) {}

  bool scatter(const ray_3 &incoming_ray,
               const ray_3_intersection &intersection, color &out_attenuation,
               ray_3 &out_scattered_ray) const override {
    auto scatter_direction =
        intersection.normal + vector_3::random_on_unit_sphere();
    if (approximately_equals(scatter_direction, vector_3()))
      scatter_direction = intersection.normal;

    out_scattered_ray = ray_3(intersection.point, scatter_direction);
    out_attenuation = albedo_;

    return true;
  }

private:
  color albedo_;
};

class metal : public material {
public:
  metal(const color &albedo) : albedo_(albedo) {}

  bool scatter(const ray_3 &incoming_ray,
               const ray_3_intersection &intersection, color &out_attenuation,
               ray_3 &out_scattered_ray) const override {
    auto scatter_direction =
        reflect(incoming_ray.direction(), intersection.normal);

    out_scattered_ray = ray_3(intersection.point, scatter_direction);
    out_attenuation = albedo_;

    return true;
  }

private:
  color albedo_;
};
