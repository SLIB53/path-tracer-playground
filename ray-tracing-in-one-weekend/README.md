# Ray Tracing in One Weekend

![demo](./demo/demo-5k-512-samples-per-pixel-64-bounces-per-ray.png)

This is my solution to [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html), the first book in the Ray Tracing in One Weekend book series! You can find my [book review](https://foodandwires.blog/p/ray-tracing-in-one-weekend-review) on my blog.

Though I used C++23 and deviate significantly from the book, the solution produces the same quality results as the book's reference renders. After completing the implementation, I had some fun tweaking the final scene as well. The result above was rendered at 5,120 by 2,880 pixels with 512 samples per pixel and up to 64 bounces per ray.

## Implementation Differences

- Renders with tile-based multi-threading.
- Path tracing algorithm is iterative with O(1) memory complexity, rather than recursive with O(n) memory complexity where n is the number of bounces.
- C++23
  - Tracing uses C++ concepts to intersect with geometry, preferring static dispatch instead of dynamic dispatch.
- Ray generation logic is part of the `ray_tracer` class and moved out of the `camera` class.
- Preferred assertions to defensive programming.
- The final render uses a color palette, among other cosmetic changes. You can read more about it in my [blog post](https://foodandwires.blog/p/orbs).

And many more...

## Build & Run

**Requirements:**

- clang
- cmake
- ninja

**Build Targets:**

- `debug`
  - used as clangd compilation target
- `release`

### Guide

To build, first generate `./build`:

```sh
cmake --preset debug; cmake --preset release
```

To build and run, use the desired preset and pipe output to a pixmap file:

```sh
cmake --build --preset debug
./build/debug/ray-tracing-in-one-weekend > demo.ppm
```

```sh
cmake --build --preset release
./build/release/ray-tracing-in-one-weekend > demo.ppm
```


## Modifications

You can tweak the render parameters in [`main.cpp`](./src/main.cpp). To change the color palette, see [`color.h`](./src/color.h).

You can use the following Python script to generate the array from a list of hexadecimal colors:

```python
from decimal import ROUND_HALF_UP, Decimal
from itertools import islice


def parse_hex(text: str) -> list[str]:
    return list(islice((c for c in text if c.isalnum()), 6))


def format_parsed_hex_to_color(hex_parsed: list[str]) -> str:
    def decode_and_quantize_parsed_channel(channel_parsed: list[str]) -> Decimal:
        return (
            Decimal(int("".join(channel_parsed), 16)) / Decimal(255)
        ).quantize(Decimal("1e-8"), rounding=ROUND_HALF_UP)

    r = decode_and_quantize_parsed_channel(hex_parsed[0:2])
    g = decode_and_quantize_parsed_channel(hex_parsed[2:4])
    b = decode_and_quantize_parsed_channel(hex_parsed[4:6])

    return f"color({r:.8f}, {g:.8f}, {b:.8f})"


def format_parsed_hex(hex_parsed: list[str]) -> str:
    return ''.join(hex_parsed).upper()


def main():
    palette = [
        ("#FBF1C7", "black"),
        ("#CC241D", "red"),
        ("#98971A", "green"),
        ("#D79921", "yellow"),
        ("#458588", "blue"),
        ("#B16286", "magenta"),
        ("#689D6A", "cyan"),
        ("#7C6F64", "white"),
        ("#928374", "bright black"),
        ("#9D0006", "bright red"),
        ("#79740E", "bright green"),
        ("#B57614", "bright yellow"),
        ("#076678", "bright blue"),
        ("#8F3F71", "bright magenta"),
        ("#427B58", "bright cyan"),
        ("#3C3836", "bright white"),
    ]

    print("inline constexpr std::array<color, 16> base16_palette{")
    for index, (hex_parsed, label) in enumerate(
        (parse_hex(hex_token), label) for (hex_token, label) in palette
    ):
        color_formatted = format_parsed_hex_to_color(hex_parsed)
        hex_formatted = format_parsed_hex(hex_parsed)

        print(
            f"{'':4}{color_formatted}, // {index:>2} #{hex_formatted} {label}"
        )
    print("};")


if __name__ == "__main__":
    main()
```
