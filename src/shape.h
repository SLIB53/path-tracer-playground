#pragma once

#include "ray_3_intersectable.h"

using shape = any_ray_3_intersectable;

template <class T>
concept shape_range = std::ranges::forward_range<T> &&
                      std::same_as<std::ranges::range_value_t<T>, shape>;
