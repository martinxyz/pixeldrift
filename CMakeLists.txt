cmake_minimum_required(VERSION 3.8)
project(pixeldrift)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Generate python module
add_subdirectory(external/pybind11)
pybind11_add_module(pixeldrift ${SOURCES} "src/pixeldrift/bindings.cpp")

target_include_directories(pixeldrift PRIVATE "src/pixeldrift")
target_include_directories(pixeldrift PRIVATE "external/entt/src")

set_property(TARGET pixeldrift PROPERTY CXX_STANDARD 20)
set_property(TARGET pixeldrift PROPERTY CXX_STANDARD_REQUIRED ON)

target_compile_options(pixeldrift PRIVATE -Wall -Werror -Wextra)
target_compile_options(pixeldrift PRIVATE -ffast-math)
