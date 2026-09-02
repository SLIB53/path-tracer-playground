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


def parse_hex_token(hex_token: str) -> list[str]:
    return list(islice((c for c in hex_token if c.isalnum()), 6))


def parsed_hex_token_to_color(hex_token_parsed: list[str]) -> str:
    def decode(channel: list[str]) -> Decimal:
        return (Decimal(int("".join(channel), 16)) / Decimal(255)).quantize(
            Decimal("1e-8"), rounding=ROUND_HALF_UP
        )

    r_decoded = decode(hex_token_parsed[0:2])
    g_decoded = decode(hex_token_parsed[2:4])
    b_decoded = decode(hex_token_parsed[4:6])

    return f"color({r_decoded:.8f}, {g_decoded:.8f}, {b_decoded:.8f})"


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
    for index, (parsed_hex_token, label) in enumerate(
        (parse_hex_token(hex_token), label) for (hex_token, label) in palette
    ):
        print(
            f"{'':4}{(parsed_hex_token_to_color(parsed_hex_token))}, // {index:>2} #{''.join(parsed_hex_token).upper()} {label}"
        )
    print("};")


if __name__ == "__main__":
    main()
```