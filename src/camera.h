#pragma once

#include "point_3.h"

class [[nodiscard]] camera {
public:
  point_3 station_point;
  double focal_length = 0.0;
  double viewport_height = 0.0;
  double viewport_width = 0.0;

  vector_3 viewport_u() const noexcept {
    return vector_3(viewport_width, 0.0, 0.0);
  }

  vector_3 viewport_v() const noexcept {
    return vector_3(0.0, -viewport_height, 0.0);
  }

  point_3 viewport_origin() const noexcept {
    return station_point - vector_3(0.0, 0.0, focal_length) -
           viewport_u() / 2.0 - viewport_v() / 2.0;
  }
};
