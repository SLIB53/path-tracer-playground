#pragma once

#include <print>

#include "camera.h"
#include "shape.h"

class pixmap_formatter {
public:
  unsigned int image_width = 0;
  unsigned int image_height = 0;

  [[nodiscard]] std::string format_header() const {
    return std::format("P3\n{} {}\n{}", image_width, image_height,
                       max_color_level);
  }

  [[nodiscard]] std::string format_pixel(const color &pixel_color) const {
    auto color_channel_to_level = [](double channel) noexcept -> int {
      return int((double(max_color_level) + 0.999) * channel);
    };

    return std::format("{} {} {}", color_channel_to_level(pixel_color.r()),
                       color_channel_to_level(pixel_color.g()),
                       color_channel_to_level(pixel_color.b()));
  }

private:
  static constexpr int max_color_level = 255;
};

class palette {
public:
  [[nodiscard]] static constexpr const color &at(std::size_t i) {
    return colors_.at(i);
  }

  [[nodiscard]] static constexpr const color &random_standard() {
    thread_local std::mt19937 generator{std::random_device{}()};
    thread_local std::uniform_int_distribution<std::size_t> distribution(0, 7);

    return colors_.at(distribution(generator));
  }

  [[nodiscard]] static constexpr const color &random_bright() {
    thread_local std::mt19937 generator{std::random_device{}()};
    thread_local std::uniform_int_distribution<std::size_t> distribution(8, 15);

    return colors_.at(distribution(generator));
  }

private:
  /* Gruvbox Light Hard */
  static constexpr std::array<color, 16> colors_{
      color(0.984314f, 0.945098f, 0.780392f), //  0 black          #FBF1C7
      color(0.800000f, 0.141176f, 0.113725f), //  1 red            #CC241D
      color(0.596078f, 0.592157f, 0.101961f), //  2 green          #98971A
      color(0.843137f, 0.600000f, 0.129412f), //  3 yellow         #D79921
      color(0.270588f, 0.521569f, 0.533333f), //  4 blue           #458588
      color(0.694118f, 0.384314f, 0.525490f), //  5 magenta        #B16286
      color(0.407843f, 0.615686f, 0.415686f), //  6 cyan           #689D6A
      color(0.486275f, 0.435294f, 0.392157f), //  7 white          #7C6F64
      color(0.572549f, 0.513725f, 0.454902f), //  8 bright black   #928374
      color(0.615686f, 0.000000f, 0.023529f), //  9 bright red     #9D0006
      color(0.474510f, 0.454902f, 0.054902f), // 10 bright green   #79740E
      color(0.709804f, 0.462745f, 0.078431f), // 11 bright yellow  #B57614
      color(0.027451f, 0.400000f, 0.470588f), // 12 bright blue    #076678
      color(0.560784f, 0.247059f, 0.443137f), // 13 bright magenta #8F3F71
      color(0.258824f, 0.482353f, 0.345098f), // 14 bright cyan    #427B58
      color(0.235294f, 0.219608f, 0.211765f), // 15 bright white   #3C3836
  };
};

// Given a trace ray, interpolate a gradient.
inline color background_color(const ray_3 &trace_ray) {
  auto a = (normalize(trace_ray.direction()).y() + 1.0) / 2.0;

  color result = (1.0 - a) * color(1.0, 1.0, 1.0) + a * palette::at(8);
  assert_color(result);

  return result;
}

// Given (u, v) coordinates of a viewport pixel, sample the world from the
// coordinates.
inline color trace_color(const pixmap_formatter &formatter,
                         const camera &main_camera,
                         const shape_range auto &world, double u, double v) {
  static constexpr unsigned int max_trace_depth = 64;

  ray_3 trace_tail;
  {
    auto pixel_delta_u = main_camera.viewport_u() / formatter.image_width,
         pixel_delta_v = main_camera.viewport_v() / formatter.image_height;

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
    for (unsigned int trace_depth = 0; trace_depth < max_trace_depth;
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

// Given the row and column of a formatter pixel, average samples of the world
// from the pixel.
inline color super_sampled_pixel_color(const pixmap_formatter &formatter,
                                       const camera &main_camera,
                                       const shape_range auto &world,
                                       unsigned int row, unsigned int column) {
  static constexpr unsigned int max_samples = 512;

  vector_3 sample_color_sum;
  for (unsigned int sample = 0; sample < max_samples; ++sample) {
    double u_jittered, v_jittered;
    {
      thread_local std::mt19937 generator{std::random_device{}()};
      thread_local std::uniform_real_distribution<double> distribution(0.0,
                                                                       1.0);
      u_jittered = column + distribution(generator) - 0.5,
      v_jittered = row + distribution(generator) - 0.5;
    }

    sample_color_sum +=
        trace_color(formatter, main_camera, world, u_jittered, v_jittered);
  }

  color result = sample_color_sum / max_samples;
  assert_color(result);

  return result;
}

inline void render(const pixmap_formatter &formatter, const camera &main_camera,
                   const shape_range auto &world) {
  std::println("{}", formatter.format_header());

  for (unsigned int row = 0; row < formatter.image_height; ++row) {
    std::print(stderr, "\r\x1b[KRows of pixels remaining: {}/{}",
               formatter.image_height - row, formatter.image_height);

    for (unsigned int column = 0; column < formatter.image_width; ++column)
      std::println("{}", formatter.format_pixel(
                             linear_to_gamma_color(super_sampled_pixel_color(
                                 formatter, main_camera, world, row, column))));
  }
}
