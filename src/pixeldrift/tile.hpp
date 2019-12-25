#pragma once
#include <cstdint>

constexpr int tile_size_log2 = 9;
constexpr int tile_size = (1 << tile_size_log2);

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

struct TileContent {
  uint8_t block : 4;
  uint8_t direction : 3;
  uint8_t particle : 1;
};

// struct Tile {
//   TileContent data[tile_size * tile_size];
// };
