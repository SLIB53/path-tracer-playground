#pragma once

#include "color.h"
#include "interval.h"
#include "ray_3_intersection.h"

class [[nodiscard]] material {
public:
  virtual ~material() = default;

  virtual bool scatter([[maybe_unused]] const ray_3 &incoming_ray,
                       [[maybe_unused]] const ray_3_intersection &intersection,
                       [[maybe_unused]] color &out_attenuation,
                       [[maybe_unused]] ray_3 &out_scattered_ray) const {
    return false;
  }
};

class [[nodiscard]] lambertian : public material {
public:
  explicit lambertian(const color &albedo) noexcept : albedo_(albedo) {}

  bool scatter(const ray_3 &, const ray_3_intersection &intersection,
               color &out_attenuation,
               ray_3 &out_scattered_ray) const override {
    auto scatter_direction =
        intersection.normal + vector_3::random_on_unit_sphere();
    if (approximately_equals(scatter_direction, vector_3()))
      scatter_direction = intersection.normal;

    out_attenuation = albedo_;
    out_scattered_ray = ray_3(intersection.point, scatter_direction);

    return true;
  }

private:
  color albedo_;
};

class [[nodiscard]] metal : public material {
public:
  metal(const color &albedo, double fuzz) noexcept
      : albedo_(albedo), fuzz_(fuzz) {}

  bool scatter(const ray_3 &incoming_ray,
               const ray_3_intersection &intersection, color &out_attenuation,
               ray_3 &out_scattered_ray) const override {
    auto reflection =
        normalize(reflect(incoming_ray.direction(), intersection.normal));

    auto scatter_direction =
        reflection + fuzz_ * vector_3::random_on_unit_hemisphere(reflection);

    out_attenuation = albedo_;
    out_scattered_ray = ray_3(intersection.point, scatter_direction);

    return true;
  }

private:
  color albedo_;
  double fuzz_;
};

class [[nodiscard]] dielectric : public material {
public:
  explicit dielectric(double refraction_index) noexcept
      : refraction_index_(refraction_index) {}

  bool scatter(const ray_3 &incoming_ray,
               const ray_3_intersection &intersection, color &out_attenuation,
               ray_3 &out_scattered_ray) const override {
    out_attenuation = color(1.0, 1.0, 1.0);

    // out_scattered_ray
    {
      auto refraction_index_of_intersection = intersection.from_front
                                                  ? (1.0 / refraction_index_)
                                                  : refraction_index_;

      auto incoming_ray_direction_normalized =
          normalize(incoming_ray.direction());

      bool reflect_incoming_ray;
      {
        auto cos_theta_from_intersection_normal = std::fmin(
            dot(-incoming_ray_direction_normalized, intersection.normal), 1.0);

        auto sin_theta_from_intersection_normal =
            std::sqrt(1.0 - cos_theta_from_intersection_normal *
                                cos_theta_from_intersection_normal);

        bool total_internal_reflection =
            refraction_index_of_intersection *
                sin_theta_from_intersection_normal >
            1.0;

        auto r = (1 - refraction_index_of_intersection) /
                 (1 + refraction_index_of_intersection);
        r *= r;
        auto schlick_reflectance =
            r + (1 - r) * std::pow((1 - cos_theta_from_intersection_normal), 5);

        bool schlick_reflection =
            schlick_reflectance > random_in_interval(zero_to_one);

        reflect_incoming_ray = total_internal_reflection || schlick_reflection;
      }

      vector_3 out_direction;
      {
        out_direction = reflect_incoming_ray
                            ? reflect(incoming_ray_direction_normalized,
                                      intersection.normal)
                            : refract(incoming_ray_direction_normalized,
                                      intersection.normal,
                                      refraction_index_of_intersection);
      }

      out_scattered_ray = ray_3(intersection.point, out_direction);
    }

    return true;
  }

private:
  double refraction_index_;
};
