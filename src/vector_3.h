#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <format>
#include <random>

class vector_3;

constexpr vector_3 operator/(const vector_3 &, double) noexcept;

constexpr double dot(const vector_3 &, const vector_3 &) noexcept;

class [[nodiscard]] vector_3 {
public:
  std::array<double, 3> elements;

  constexpr vector_3() noexcept : elements{0.0, 0.0, 0.0} {}

  constexpr vector_3(double element1, double element2, double element3) noexcept
      : elements{element1, element2, element3} {}

  [[nodiscard]] constexpr double x() const noexcept { return elements[0]; }
  [[nodiscard]] constexpr double y() const noexcept { return elements[1]; }
  [[nodiscard]] constexpr double z() const noexcept { return elements[2]; }

  [[nodiscard]] constexpr double r() const noexcept { return elements[0]; }
  [[nodiscard]] constexpr double g() const noexcept { return elements[1]; }
  [[nodiscard]] constexpr double b() const noexcept { return elements[2]; }

  constexpr vector_3 operator-() const noexcept {
    return vector_3(-elements[0], -elements[1], -elements[2]);
  }

  [[nodiscard]] constexpr double operator[](int i) const noexcept {
    assert(i >= 0 && i < 3);
    return elements[i];
  }

  [[nodiscard]] constexpr double &operator[](int i) noexcept {
    assert(i >= 0 && i < 3);
    return elements[i];
  }

  constexpr vector_3 &operator+=(const vector_3 &v) noexcept {
    elements[0] += v.elements[0];
    elements[1] += v.elements[1];
    elements[2] += v.elements[2];

    return *this;
  }

  constexpr vector_3 &operator-=(const vector_3 &v) noexcept {
    elements[0] -= v.elements[0];
    elements[1] -= v.elements[1];
    elements[2] -= v.elements[2];

    return *this;
  }

  constexpr vector_3 &operator*=(double t) noexcept {
    elements[0] *= t;
    elements[1] *= t;
    elements[2] *= t;

    return *this;
  }

  constexpr vector_3 &operator/=(double t) noexcept { return *this *= 1 / t; }

  [[nodiscard]] constexpr double length_squared() const noexcept {
    return elements[0] * elements[0] + elements[1] * elements[1] +
           elements[2] * elements[2];
  }

  [[nodiscard]] double length() const noexcept {
    return std::sqrt(length_squared());
  }

  static vector_3 random(double min = 0.0, double max = 1.0) {
    thread_local std::mt19937 generator{std::random_device{}()};
    std::uniform_real_distribution<double> distribution(min, max);

    return vector_3(distribution(generator), distribution(generator),
                    distribution(generator));
  }

  static vector_3 random_on_unit_sphere() {
    constexpr auto delta = 1e-160;

    while (true) {
      auto candidate = vector_3::random(-1.0, 1.0);

      if (auto candidate_length_squared = candidate.length_squared();
          candidate_length_squared > delta && candidate_length_squared <= 1.0)
        return candidate / std::sqrt(candidate_length_squared);
    }
  }

  static vector_3 random_on_unit_hemisphere(const vector_3 &normal) {
    auto candidate = vector_3::random_on_unit_sphere();

    return dot(candidate, normal) > 0.0 ? candidate : -candidate;
  }
};

template <> struct std::formatter<vector_3> : std::formatter<double> {
  auto format(const vector_3 &v, std::format_context &ctx) const {
    auto out = std::formatter<double>::format(v.elements[0], ctx);
    *out++ = ' ';
    out = std::formatter<double>::format(v.elements[1], ctx);
    *out++ = ' ';
    return std::formatter<double>::format(v.elements[2], ctx);
  }
};

constexpr vector_3 operator+(const vector_3 &u, const vector_3 &v) noexcept {
  return vector_3(u.elements[0] + v.elements[0], u.elements[1] + v.elements[1],
                  u.elements[2] + v.elements[2]);
}

constexpr vector_3 operator-(const vector_3 &u, const vector_3 &v) noexcept {
  return vector_3(u.elements[0] - v.elements[0], u.elements[1] - v.elements[1],
                  u.elements[2] - v.elements[2]);
}

constexpr vector_3 operator*(double t, const vector_3 &v) noexcept {
  return vector_3(t * v.elements[0], t * v.elements[1], t * v.elements[2]);
}

constexpr vector_3 operator*(const vector_3 &v, double t) noexcept {
  return t * v;
}

constexpr vector_3 operator/(const vector_3 &v, double t) noexcept {
  return (1 / t) * v;
}

[[nodiscard]] constexpr bool approximately_equals(const vector_3 &u,
                                                  const vector_3 &v) noexcept {
  static constexpr auto tolerance = 1e-8;

  auto difference = u - v;

  return std::fabs(difference[0]) < tolerance &&
         std::fabs(difference[1]) < tolerance &&
         std::fabs(difference[2]) < tolerance;
}

[[nodiscard]] inline vector_3 norm(const vector_3 &v) noexcept {
  return v / v.length();
}

[[nodiscard]] constexpr double dot(const vector_3 &u,
                                   const vector_3 &v) noexcept {
  return u.elements[0] * v.elements[0] + u.elements[1] * v.elements[1] +
         u.elements[2] * v.elements[2];
}

[[nodiscard]] constexpr vector_3 cross(const vector_3 &u,
                                       const vector_3 &v) noexcept {
  return vector_3(u.elements[1] * v.elements[2] - u.elements[2] * v.elements[1],
                  u.elements[2] * v.elements[0] - u.elements[0] * v.elements[2],
                  u.elements[0] * v.elements[1] -
                      u.elements[1] * v.elements[0]);
}

[[nodiscard]] constexpr vector_3 reflect(const vector_3 &v,
                                         const vector_3 &n) noexcept {
  return v - 2 * dot(v, n) * n;
}

[[nodiscard]] constexpr vector_3 pairwise_multiply(const vector_3 &u,
                                                   const vector_3 &v) noexcept {
  return vector_3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}
