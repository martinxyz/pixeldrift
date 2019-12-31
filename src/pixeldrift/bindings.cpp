#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <cstdint>
#include "systems.hpp"
#include "turing_head.hpp"

namespace py = pybind11;

using array_u8 = py::array_t<uint8_t, py::array::c_style>;

array_u8 get_particles(World &world, int x0, int y0, int w, int h) {
  if (w <= 0) throw std::invalid_argument("w must be positive");
  if (h <= 0) throw std::invalid_argument("h must be positive");
  auto result = array_u8(h*w);
  result.resize({h, w});

  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      // API uses offset coordinates, while tile storage is axial
      int x_axial = x0 + x - static_cast<unsigned>(y0 + y)/2;
      int y_axial = y0 + y;
      uint8_t value = world.map.at(x_axial, y_axial).particle ? 1 : 0;
      result.mutable_at(y, x) = value;
    }
  }
  return result;
}

void set_particles(World &world, array_u8 &arr, int x0, int y0) {
  auto info = arr.request();
  if (info.ndim != 2) {
    throw std::invalid_argument("wrong number of dimensions");
  }
  int h = info.shape[0];
  int w = info.shape[1];

  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      // API uses offset coordinates, while tile storage is axial
      int x_axial = x0 + x - static_cast<unsigned>(y0 + y)/2;
      int y_axial = y0 + y;
      uint8_t value = arr.at(y, x) ? 1 : 0;
      world.map.at(x_axial, y_axial).particle = value;
    }
  }
}

void turing_head_set_lut(TuringHeads &th, array_u8 &arr) {
  auto info = arr.request();
  if (info.shape[0] != TuringHeads::lut_size) {
    throw std::invalid_argument("wrong size of lut array");
  }
  th.set_lut(info.ptr);
}

void apply_lut_filter(World &world, array_u8 &arr) {
  auto info = arr.request();
  if (info.shape[0] != 1 << 7) {
    throw std::invalid_argument("wrong size of lut array");
  }
  world.apply_lut_filter(static_cast<uint8_t*>(info.ptr));
}

PYBIND11_MODULE(pixeldrift, m) {
  m.attr("tile_size") = Tile::size;

  py::class_<World>(m, "World")
      .def(py::init<>())
      .def("get_particles", &get_particles)
      .def("set_particles", &set_particles)
      .def("apply_lut_filter", &apply_lut_filter)
      .def("tick", &World::tick);

  py::class_<TuringHeads>(m, "TuringHeads")
      .def(py::init<>())
      .def("add_instance", &TuringHeads::add_instance)
      .def("set_lut", &turing_head_set_lut);
}
