cmake_minimum_required(VERSION 3.8)
project(pixeldrift)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(SOURCE_DIR "src/pixeldrift")
# Tell cmake that headers are in alse in source_dir
include_directories(${SOURCE_DIR})

# Generate python module
add_subdirectory(external/pybind11)
pybind11_add_module(pixeldrift ${SOURCES} "${SOURCE_DIR}/bindings.cpp")

# https://community.arm.com/developer/tools-software/tools/b/tools-software-ides-blog/posts/compiler-flags-across-architectures-march-mtune-and-mcpu
# on ARM: -mcpu=native
# on x86: -march=native
