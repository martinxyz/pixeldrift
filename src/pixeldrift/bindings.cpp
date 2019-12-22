#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <cstdint>
#include "systems.hpp"

namespace py = pybind11;

using array_u8 = py::array_t<uint8_t, py::array::c_style>;

static std::array<uint16_t, 3> sdata{4,5,6};

auto f(array_u8 &arr) {
  // auto info = arr.request();
  // arr[2] = 3;
  arr.mutable_at(5) = arr.at(5) + 2;
  // return arr.at(5); // info.shape[0];
  // auto res = py::array_t<int16_t>({5});
  // auto res = py::array_t<uint16_t>({3}, {}, sdata);
  auto res = py::array_t<uint16_t>({}, {}, sdata.data());
  return py::array_t<uint8_t>(sdata);
  // arr.mutable_at(1) = 99;
  // return res;
  // return 5;
}

PYBIND11_MODULE(pixeldrift, m) {
  // py::module m("pixeldrift");

  m.def("f", &f);

  py::class_<DiffusionTile>(m, "DiffusionTile")
      .def(py::init<>());
      // .def_readonly_static("size", &DiffusionMap::size);
      // .def_readwrite("arr", &DiffusionTile::arr);
}
