#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <cstdint>
#include "systems.hpp"
#include "turing_head.hpp"

namespace py = pybind11;

using array_u8 = py::array_t<uint8_t, py::array::c_style>;

CellContent get_cell(World &world, int x, int y) {
  int x_axial = x - static_cast<unsigned>(y)/2;
  int y_axial = y;
  return world.map.at(x_axial, y_axial);
}

void set_cell(World &world, int x, int y, CellContent value) {
  int x_axial = x - static_cast<unsigned>(y)/2;
  int y_axial = y;
  world.map.at(x_axial, y_axial) = value;
}

template <int channel>
array_u8 get_channel(World &world, int x0, int y0, int w, int h) {
  if (w <= 0) throw std::invalid_argument("w must be positive");
  if (h <= 0) throw std::invalid_argument("h must be positive");
  auto result = array_u8(h*w);
  result.resize({h, w});

  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      // API uses offset coordinates, while tile storage is axial
      int x_axial = x0 + x - static_cast<unsigned>(y0 + y)/2;
      int y_axial = y0 + y;
      CellContent &cc = world.map.at(x_axial, y_axial);
      uint8_t value = 255;
      if (channel == 0) value = cc.particle ? 1 : 0;
      if (channel == 1) value = cc.cell_type;
      result.mutable_at(y, x) = value;   // lots of load instructions (numpy shape, writeable check, etc.)
    }
  }
  return result;
}

template <int channel>
void set_channel(World &world, array_u8 &arr, int x0, int y0) {
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
      CellContent &cc = world.map.at(x_axial, y_axial);
      if (channel == 0) cc.particle = value;
      if (channel == 1) {
        cc.cell_type = value;
        cc.child1_count = 0;
      }
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

double count_lut_filter(World &world, array_u8 &arr) {
  auto info = arr.request();
  if (info.shape[0] != 1 << 7) {
    throw std::invalid_argument("wrong size of lut array");
  }
  return world.count_lut_filter(static_cast<uint8_t*>(info.ptr));
}

PYBIND11_MODULE(pixeldrift, m) {
  m.attr("tile_size") = Tile::size;

  py::class_<CellType>(m, "CellType")
      .def(py::init<>())
      .def_readwrite("child1", &CellType::child1)
      .def_readwrite("child2", &CellType::child2)
      .def_readwrite("child1_maxcount", &CellType::child1_maxcount);

  py::class_<CellContent>(m, "CellContent")
      .def(py::init<>())
      .def_readwrite("cell_type", &CellContent::cell_type)
      .def_readwrite("child1_count", &CellContent::child1_count)
      .def_property("particle",
                    [](const CellContent &cc) { return cc.particle ? true : false; },
                    [](CellContent &cc, bool value) { cc.particle = value ? 1 : 0; }
                    );

  py::class_<World>(m, "World")
      .def(py::init<>())

      .def_readwrite("cell_types", &World::cell_types)
      .def("get_cell", &get_cell)
      .def("set_cell", &set_cell)
      .def("tick6", &World::tick6)
      .def("tick6_single", &World::tick6_single)

      .def("get_particles", &get_channel<0>)
      .def("set_particles", &set_channel<0>)
      .def("get_cell_types", &get_channel<1>)
      .def("set_cell_types", &set_channel<1>)
      .def("apply_lut_filter", &apply_lut_filter)
      .def("count_lut_filter", &count_lut_filter)
      .def("tick", &World::tick);

  py::class_<TuringHeads>(m, "TuringHeads")
      .def(py::init<>())
      .def("add_instance", &TuringHeads::add_instance)
      .def("set_lut", &turing_head_set_lut);
}
