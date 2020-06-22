# pixeldrift

Has been ported to Rust and continued here: https://github.com/martinxyz/progenitor

This repo is abandoned.

Still good as a template for a mixed C++/Python project. Note: setting up a
mixed Rust/Python project was so much easier in comparison, the build setup was
almost a no-op.

# Build and Install

```bash
git submodule update --init --recursive

# OPTIONAL: set optimization flags (default is just -O3)
# on ARM: -mcpu=native
# on x86: -march=native
# remove the 'build' directory after changing this
#export CXXFLAGS='-march=native'

pip3 install -v -e .
```

# Tests
To execute all unit tests:

```bash
python3 ./setup.py test
```

# Requirements

- Python 3
- CMake 3.8 or higher
- Compiler with C++17 support

# Acknowledgements

The build system is based on [python_cpp_example](http://www.benjack.io/2018/02/02/python-cpp-revisited.html) by Benjamin R. Jack.
