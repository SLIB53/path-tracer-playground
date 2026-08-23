cmake_minimum_required(VERSION 4.2...4.4)

project(PathtracerPlayground LANGUAGES CXX)

add_executable(path-tracer-playground src/main.cpp)

target_include_directories(path-tracer-playground PRIVATE src)

target_compile_features(path-tracer-playground PRIVATE cxx_std_23)

target_compile_options(path-tracer-playground PRIVATE $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra -Wpedantic>)
