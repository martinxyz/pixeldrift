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
  // API uses offset coordinates, while tile storage is axial
  x0 -= static_cast<unsigned>(y0)/2;
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      uint8_t value = world.map.at((x+x0)-static_cast<unsigned>(y+y0)/2, (y+y0)).particle ? 1 : 0;
      result.mutable_at(y, x) = value;
    }
  }
  return result;
}

void set_particles(World &world, array_u8 &arr, int x0, int y0) {
  auto info = arr.request();
  if (info.ndim != 2) {
    throw std::out_of_range("wrong number of dimensions");
  }
  int h = info.shape[0];
  int w = info.shape[1];

  // API uses offset coordinates, while tile storage is axial
  x0 -= static_cast<unsigned>(y0)/2;

  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      uint8_t value = arr.at(y, x) ? 1 : 0;
      world.map.at((x+x0)-static_cast<unsigned>(y+y0)/2, (y+y0)).particle = value;
    }
  }
}

void turing_head_set_lut(TuringHeads &th, array_u8 &arr) {
  auto info = arr.request();
  if (info.shape[0] != TuringHeads::lut_size) {
    throw std::out_of_range("wrong shape of lut array");
  }
  th.set_lut(info.ptr);
}

PYBIND11_MODULE(pixeldrift, m) {
  m.attr("tile_size") = Tile::size;

  py::class_<World>(m, "World")
      .def(py::init<>())
      .def("get_particles", &get_particles)
      .def("set_particles", &set_particles)
      .def("tick", &World::tick);

  py::class_<TuringHeads>(m, "TuringHeads")
      .def(py::init<>())
      .def("add_instance", &TuringHeads::add_instance)
      .def("set_lut", &turing_head_set_lut);
}
