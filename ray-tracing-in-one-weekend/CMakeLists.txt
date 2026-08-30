cmake_minimum_required(VERSION 4.2...4.4)

project(RayTracingInOneWeekend LANGUAGES CXX)

add_executable(ray-tracing-in-one-weekend src/main.cpp)

target_include_directories(ray-tracing-in-one-weekend PRIVATE src)

target_compile_features(ray-tracing-in-one-weekend PRIVATE cxx_std_23)

target_compile_options(ray-tracing-in-one-weekend PRIVATE $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra -Wpedantic>)
