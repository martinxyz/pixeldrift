#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <cstdint>
#include "systems.hpp"
#include "turing_head.hpp"

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

auto extract_tile(array_u8 &arr) {
  // auto info = arr.request();
  // arr[2] = 3;
  arr.mutable_at(5) = arr.at(5) + 2;
  // return arr.at(5); // info.shape[0];
  // auto res = py::array_t<int16_t>({5});
  // auto res = py::array_t<uint16_t>({3}, {}, sdata);
  auto res = py::array_t<uint16_t>({}, {}, sdata.data());
  // auto res = array_u8<uint8_t>(sdata);
  // arr.mutable_at(1) = 99;
  // return res;
  // return 5;
}

array_u8 get_particles(World &world) {
  constexpr int N = Tile::size;
  auto result = array_u8(N*N);
  auto buf = static_cast<uint8_t*>(result.request().ptr);
  for (int y=0; y<N; y++) {
    for (int x=0; x<N; x++) {
      buf[y*N+x] = world.map.at(x, y).particle ? 255 : 0;
    }
  }
  result.resize({N, N});
  return result;
}

void turing_head_set_lut(TuringHeads &th, array_u8 &arr) {
  auto info = arr.request();
  if (info.shape[0] != TuringHeads::lut_size) abort();
  th.set_lut(info.ptr);
}

PYBIND11_MODULE(pixeldrift, m) {
  m.attr("tile_size") = Tile::size;

  // py::class_<TileContent>(m, "TileContent");
  py::class_<World>(m, "World")
      .def(py::init<>())
      .def("get_particles", &get_particles)
      .def("tick", &World::tick);
      // .def_readonly("map", &World::map);
  // .def_readonly_static("size", &DiffusionMap::size);
  // .def_readwrite("arr", &DiffusionTile::arr);

  py::class_<TuringHeads>(m, "TuringHeads")
      .def(py::init<>())
      .def("add_head", &TuringHeads::add_head)
      .def("set_lut", &turing_head_set_lut);
}
