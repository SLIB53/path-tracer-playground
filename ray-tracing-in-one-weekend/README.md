# Ray Tracing in One Weekend

![demo](./demo/demo-5k-512-samples-per-pixel-64-bounces-per-ray.png)

This is my solution to [Ray Tracing in One Weekend](https://raytracing.github.io/books/RayTracingInOneWeekend.html), the first book in the Ray Tracing in One Weekend book series!

Though I used C++23 and deviate significantly from the book, the solution produces the same quality results as the book's reference renders. After completing the implementation, I had some fun tweaking the final scene as well. The result above was rendered at 5,120 by 2,880 pixels with 512 samples per pixel and up to 64 bounces per ray.

## Implementation Differences

- Renders with tile-based multi-threading.
- Path tracing algorithm is iterative with O(1) memory complexity, rather than recursive with O(n) memory complexity where n is the number of bounces.
- C++23
  - Tracing uses C++ concepts to intersect with geometry, preferring static dispatch instead of dynamic dispatch.
- Ray generation logic is part of the `ray_tracer` class and moved out of the `camera` class.
- Preferred assertions to defensive programming.

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
