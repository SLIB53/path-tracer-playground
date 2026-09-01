#pragma once

#include "color.h"

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
