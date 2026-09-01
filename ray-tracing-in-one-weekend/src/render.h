#pragma once

#include <print>

#include "camera.h"
#include "palette.h"
#include "shape.h"

// Given a trace ray, interpolate a gradient.
inline color background_color(const ray_3 &trace_ray) {
  auto a = (normalize(trace_ray.direction()).y() + 1.0) / 2.0;

  color result = (1.0 - a) * color(1.0, 1.0, 1.0) + a * palette::at(8);
  assert_color(result);

  return result;
}

// Given (u, v) coordinates of a viewport, sample the world from the
// coordinates.
inline color trace_color(unsigned image_width, unsigned image_height,
                         const camera &main_camera,
                         const shape_range auto &world, double u, double v) {
  static constexpr unsigned max_trace_depth = 64;

  ray_3 trace_tail;
  {
    auto pixel_delta_u = main_camera.viewport_u() / image_width,
         pixel_delta_v = main_camera.viewport_v() / image_height;

    auto pixel_00_center =
        main_camera.viewport_origin() + (pixel_delta_u + pixel_delta_v) / 2.0;

    auto pixel_uv_center =
        pixel_00_center + (u * pixel_delta_u) + (v * pixel_delta_v);

    auto depth_of_field_disk_r = vector_3::random_on_unit_disk();
    auto depth_of_field_disk_uv =
        depth_of_field_disk_r[0] * main_camera.depth_of_field_disk_u() +
        depth_of_field_disk_r[1] * main_camera.depth_of_field_disk_v();

    point_3 trace_tail_origin =
        main_camera.axial_ray.origin() + depth_of_field_disk_uv;
    vector_3 trace_tail_direction = pixel_uv_center - trace_tail_origin;
    trace_tail = ray_3(trace_tail_origin, trace_tail_direction);
  }

  color trace_accumulation;
  {
    // Initial value of (1, 1, 1) is fine because it won't contribute in
    // pairwise multiplication.
    trace_accumulation = color(1.0, 1.0, 1.0);

    // Because the accumulation is commutative, we can use the iterative form
    // without keeping a stack rather than accumulate recursively.
    for (unsigned trace_depth = 0; trace_depth < max_trace_depth;
         ++trace_depth) {
      ray_3_intersection intersection;
      if (!intersects(world, trace_tail, interval(0.001, +infinity),
                      intersection))
        break;

      color attenuation;
      ray_3 scattered_ray;
      if (!intersection.surface_material->scatter(trace_tail, intersection,
                                                  attenuation, scattered_ray))
        break;

      assert(!approximately_equals(scattered_ray.direction(), vector_3()));

      trace_tail = scattered_ray;
      trace_accumulation =
          component_wise_product(trace_accumulation, attenuation);
    }
  }

  // Assume the final ray reaches the background (even though this may not
  // actually be the case), and multiply the background color. This will treat
  // the background color as the ambient light.
  color result =
      component_wise_product(trace_accumulation, background_color(trace_tail));
  assert_color(result);

  return result;
}

// Given the row and column of the image, average samples of the world from the
// image's pixel at the row and column.
inline color super_sampled_pixel_color(unsigned image_width,
                                       unsigned image_height,
                                       const camera &main_camera,
                                       const shape_range auto &world,
                                       unsigned row, unsigned column) {
  static constexpr unsigned max_samples = 512;

  vector_3 sample_color_sum;
  for (unsigned sample = 0; sample < max_samples; ++sample) {
    double u_jittered, v_jittered;
    {
      thread_local std::mt19937 generator{std::random_device{}()};
      thread_local std::uniform_real_distribution<double> distribution(0.0,
                                                                       1.0);
      u_jittered = column + distribution(generator) - 0.5,
      v_jittered = row + distribution(generator) - 0.5;
    }

    sample_color_sum += trace_color(image_width, image_height, main_camera,
                                    world, u_jittered, v_jittered);
  }

  color result = sample_color_sum / max_samples;
  assert_color(result);

  return result;
}

inline void render(unsigned image_width, unsigned image_height,
                   const camera &main_camera, const shape_range auto &world,
                   std::vector<color> &out_imagebuffer) {
  for (unsigned row = 0; row < image_height; ++row) {
    std::print(stderr, "\r\x1b[K[Rendering] Rows of pixels remaining: {}/{}",
               image_height - row, image_height);

    for (unsigned column = 0; column < image_width; ++column)
      out_imagebuffer.push_back(linear_to_gamma_color(super_sampled_pixel_color(
          image_width, image_height, main_camera, world, row, column)));
  }

  std::println(stderr, "\n[Rendering] Complete.");
}
