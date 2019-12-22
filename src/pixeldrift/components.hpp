#pragma once
#include <cstdint>
#include "tile.hpp"

// struct ActionInput {
//   bool forward;
//   bool left;
//   bool right;
//   bool action;
// };

struct Vision {
  bool occupied[6];
  // bool food[6];
};

struct Position {
  int x, y;
};

struct TuringHead {
  uint32_t state;
  struct Command {
    uint8_t output : 1;
    uint8_t movement : 3;
    uint8_t state : 4;
  };
  Command * command_lut;
};

enum Component {
  kActionInput = 0,
  kVision,
  kEnergy,
  kPosition,
  kMovement,
  kTuringHead,
};

constexpr int MAX_ENTITIES = 500;
struct World {
  int mask[MAX_ENTITIES];
  // ActionInput input[MAX_ENTITIES];
  Vision vision[MAX_ENTITIES];
  Position position[MAX_ENTITIES];
  TuringHead turing_head[MAX_ENTITIES];

  TileContent map[tile_size * tile_size];
  TileContent& at(int x, int y) {
    return map[y*tile_size + x];
  }
};
