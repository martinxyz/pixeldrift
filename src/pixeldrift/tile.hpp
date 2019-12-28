#pragma once
#include <cstdint>

struct TileContent {
  uint8_t block : 4;
  uint8_t direction : 3;
  uint8_t particle : 1;
};

struct Tile {
  static constexpr int size_log2 = 9;
  static constexpr int size = (1 << size_log2);
  static constexpr int padding = 2;

  TileContent data[(size + 2*padding) * (size + 2*padding)]{};
  static constexpr int stride_x = 1;
  static constexpr int stride_y = size+2*padding;
  static constexpr int stride_z = stride_y - stride_x;
};

class TorusTileStore {
 public:
  inline TileContent& at(int x, int y) {
    bool odd = (static_cast<unsigned>(y) / Tile::size) % 2 != 0;
    if (odd) x += Tile::size/2;
    // note: can be optimized by storing pointer to inside of padding
    x = (static_cast<unsigned>(x) % Tile::size) + tile_.padding;
    y = (static_cast<unsigned>(y) % Tile::size) + tile_.padding;
    return tile_.data[y*tile_.stride_y + x];
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
