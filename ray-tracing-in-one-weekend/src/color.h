#pragma once

#include "interval.h"
#include "vector_3.h"

using color = vector_3;

// Implements gruvbox (light mode) (https://github.com/morhetz/gruvbox). The
// array was generated using a script, see the README for more information.
inline constexpr std::array<color, 16> base16_palette{
    color(0.98431373, 0.94509804, 0.78039216), //  0 #FBF1C7 black
    color(0.80000000, 0.14117647, 0.11372549), //  1 #CC241D red
    color(0.59607843, 0.59215686, 0.10196078), //  2 #98971A green
    color(0.84313725, 0.60000000, 0.12941176), //  3 #D79921 yellow
    color(0.27058824, 0.52156863, 0.53333333), //  4 #458588 blue
    color(0.69411765, 0.38431373, 0.52549020), //  5 #B16286 magenta
    color(0.40784314, 0.61568627, 0.41568627), //  6 #689D6A cyan
    color(0.48627451, 0.43529412, 0.39215686), //  7 #7C6F64 white
    color(0.57254902, 0.51372549, 0.45490196), //  8 #928374 bright black
    color(0.61568627, 0.00000000, 0.02352941), //  9 #9D0006 bright red
    color(0.47450980, 0.45490196, 0.05490196), // 10 #79740E bright green
    color(0.70980392, 0.46274510, 0.07843137), // 11 #B57614 bright yellow
    color(0.02745098, 0.40000000, 0.47058824), // 12 #076678 bright blue
    color(0.56078431, 0.24705882, 0.44313725), // 13 #8F3F71 bright magenta
    color(0.25882353, 0.48235294, 0.34509804), // 14 #427B58 bright cyan
    color(0.23529412, 0.21960784, 0.21176471), // 15 #3C3836 bright white
};

[[nodiscard]] constexpr const color &random_base16_normal_color() noexcept {
  thread_local std::mt19937 generator{std::random_device{}()};
  thread_local std::uniform_int_distribution<std::size_t> distribution(0, 7);

  return base16_palette[distribution(generator)];
}

[[nodiscard]] constexpr const color &random_base16_bright_color() noexcept {
  thread_local std::mt19937 generator{std::random_device{}()};
  thread_local std::uniform_int_distribution<std::size_t> distribution(8, 15);

  return base16_palette[distribution(generator)];
}

[[nodiscard]] inline double
linear_to_gamma_color_channel(double channel) noexcept {
  return channel > 0 ? std::sqrt(channel) : 0;
};

inline color linear_to_gamma_color(const color &clr) noexcept {
  return color(linear_to_gamma_color_channel(clr.r()),
               linear_to_gamma_color_channel(clr.g()),
               linear_to_gamma_color_channel(clr.b()));
}

inline void assert_color_channel([[maybe_unused]] double channel) noexcept {
  assert(interval(zero_to_one).contains(channel));
}

inline void assert_color([[maybe_unused]] const color &clr) noexcept {
  assert_color_channel(clr.r());
  assert_color_channel(clr.g());
  assert_color_channel(clr.b());
}
