#pragma once
#include <cstdint>
#include <vector>

struct CellType {
  uint8_t child1 = 0;
  uint8_t child2 = 0;
  uint8_t child1_maxcount = 255;
};

struct CellContent {
  uint8_t cell_type = 0;
  uint8_t child1_count = 0;
  // uint8_t direction : 3;
  uint8_t particle : 1 = 0;
  // uint8_t energy;
};

struct Tile {
  static constexpr int size_log2 = 7;
  static constexpr int size = (1 << size_log2);
  static constexpr int padding = 2;

  CellContent data[(size + 2*padding) * (size + 2*padding)]{};
  static constexpr int stride_x = 1;
  static constexpr int stride_y = size+2*padding;
  static constexpr int stride_z = stride_y - stride_x;
  CellContent * data_inside() {
    return data + padding*stride_y + padding*stride_x;
  }
};

class TorusTileStore {
 public:
  inline CellContent& at(int x, int y) {
    bool odd = (static_cast<unsigned>(y) / Tile::size) % 2 != 0;
    if (odd) x += Tile::size/2;
    // note: can be optimized by storing pointer to inside of padding
    x = (static_cast<unsigned>(x) % Tile::size) + tile_.padding;
    y = (static_cast<unsigned>(y) % Tile::size) + tile_.padding;
    return tile_.data[y*tile_.stride_y + x];
  }
  std::vector<Tile*> iter_tiles() {
    return std::vector<Tile*>{&tile_};
  }
  void update_borders() {
    constexpr int size = Tile::size;
    constexpr int padding = Tile::padding;
    constexpr int h = Tile::stride_y;
    for (int i=0; i<size + 2*padding; i++) {
      for (int k=0; k<padding; k++) {
        tile_.data[i*h + k] = tile_.data[i*h + k+size];
        tile_.data[i*h + padding+size+k] = tile_.data[i*h + padding+k];
        tile_.data[(k)*h + i] = tile_.data[(k+size)*h + (i+size/2)%size];
        tile_.data[(padding+size+k)*h + i] = tile_.data[(padding+k)*h + (i+size/2)%size];
      }
    }
  }
 private:
  Tile tile_{};
};

enum Block {
  kBorder = 0,
  kEmpty,
  kWall,
  kFood,
  kAgent,
};

enum Direction {
  kDirectionSE = 0,
  kDirectionS,
  kDirectionSW,
  kDirectionNW,
  kDirectionN,
  kDirectionNE,
  kDirectionNone,
  kDirectionNone2,
};

constexpr struct { int dx, dy; } kDirectionToDelta[]{
  {+1, 0},
  { 0,+1},
  {-1,+1},
  {-1, 0},
  { 0,-1},
  {+1,-1},
  { 0, 0},
  { 0, 0},
};
