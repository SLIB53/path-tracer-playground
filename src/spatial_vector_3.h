#ifndef SPATIAL_VECTOR_3
#define SPATIAL_VECTOR_3

#include <array>
#include <cassert>
#include <cmath>
#include <format>

class spatial_vector_3 {
public:
  std::array<double, 3> elements;

  constexpr spatial_vector_3() noexcept : elements{0, 0, 0} {}

  constexpr spatial_vector_3(double element1, double element2,
                             double element3) noexcept
      : elements{element1, element2, element3} {}

  constexpr double x() const noexcept { return elements[0]; }
  constexpr double y() const noexcept { return elements[1]; }
  constexpr double z() const noexcept { return elements[2]; }

  constexpr double r() const noexcept { return elements[0]; }
  constexpr double g() const noexcept { return elements[1]; }
  constexpr double b() const noexcept { return elements[2]; }

  constexpr spatial_vector_3 operator-() const noexcept {
    return spatial_vector_3(-elements[0], -elements[1], -elements[2]);
  }

  constexpr double operator[](int i) const noexcept {
    assert(i >= 0 && i < 3);
    return elements[i];
  }

  constexpr double &operator[](int i) noexcept {
    assert(i >= 0 && i < 3);
    return elements[i];
  }

  constexpr spatial_vector_3 &operator+=(const spatial_vector_3 &v) noexcept {
    elements[0] += v.elements[0];
    elements[1] += v.elements[1];
    elements[2] += v.elements[2];

    return *this;
  }

  constexpr spatial_vector_3 &operator-=(const spatial_vector_3 &v) noexcept {
    elements[0] -= v.elements[0];
    elements[1] -= v.elements[1];
    elements[2] -= v.elements[2];

    return *this;
  }

  constexpr spatial_vector_3 &operator*=(double t) noexcept {
    elements[0] *= t;
    elements[1] *= t;
    elements[2] *= t;

    return *this;
  }

  constexpr spatial_vector_3 &operator/=(double t) noexcept {
    return *this *= 1 / t;
  }

  constexpr double length_squared() const noexcept {
    return elements[0] * elements[0] + elements[1] * elements[1] +
           elements[2] * elements[2];
  }

  double length() const noexcept { return std::sqrt(length_squared()); }
};

template <> struct std::formatter<spatial_vector_3> : std::formatter<double> {
  auto format(const spatial_vector_3 &v, std::format_context &ctx) const {
    auto out = std::formatter<double>::format(v.elements[0], ctx);
    *out++ = ' ';
    out = std::formatter<double>::format(v.elements[1], ctx);
    *out++ = ' ';
    return std::formatter<double>::format(v.elements[2], ctx);
  }
};

// inline std::ostream &operator<<(std::ostream &out, const spatial_vector_3 &v)
// {
//   return out << std::format("{} {} {}", v.elements[0], v.elements[1],
//                             v.elements[2]);
// }

constexpr spatial_vector_3 operator+(const spatial_vector_3 &u,
                                     const spatial_vector_3 &v) noexcept {
  return spatial_vector_3(u.elements[0] + v.elements[0],
                          u.elements[1] + v.elements[1],
                          u.elements[2] + v.elements[2]);
}

constexpr spatial_vector_3 operator-(const spatial_vector_3 &u,
                                     const spatial_vector_3 &v) noexcept {
  return spatial_vector_3(u.elements[0] - v.elements[0],
                          u.elements[1] - v.elements[1],
                          u.elements[2] - v.elements[2]);
}

constexpr spatial_vector_3 operator*(double t,
                                     const spatial_vector_3 &v) noexcept {
  return spatial_vector_3(t * v.elements[0], t * v.elements[1],
                          t * v.elements[2]);
}

constexpr spatial_vector_3 operator*(const spatial_vector_3 &v,
                                     double t) noexcept {
  return t * v;
}

constexpr spatial_vector_3 operator/(const spatial_vector_3 &v,
                                     double t) noexcept {
  return (1 / t) * v;
}

constexpr double dot(const spatial_vector_3 &u,
                     const spatial_vector_3 &v) noexcept {
  return u.elements[0] * v.elements[0] + u.elements[1] * v.elements[1] +
         u.elements[2] * v.elements[2];
}

constexpr spatial_vector_3 cross(const spatial_vector_3 &u,
                                 const spatial_vector_3 &v) noexcept {
  return spatial_vector_3(
      u.elements[1] * v.elements[2] - u.elements[2] * v.elements[1],
      u.elements[2] * v.elements[0] - u.elements[0] * v.elements[2],
      u.elements[0] * v.elements[1] - u.elements[1] * v.elements[0]);
}

inline spatial_vector_3 norm(const spatial_vector_3 &v) noexcept {
  return v / v.length();
}

using point3 = spatial_vector_3;
using color = spatial_vector_3;

#endif
