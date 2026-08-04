cmake_minimum_required(VERSION 4.2...4.4)

project(PathtracerPlayground LANGUAGES CXX)

add_executable(pathtracer-playground src/main.cpp)

target_include_directories(pathtracer-playground PRIVATE src)

target_compile_features(pathtracer-playground PRIVATE cxx_std_23)

target_compile_options(pathtracer-playground PRIVATE $<$<CXX_COMPILER_ID:AppleClang,Clang,GNU>:-Wall -Wextra -Wpedantic>)
