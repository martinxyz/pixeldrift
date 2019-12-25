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
  bool occupied[7];
  // bool food[6];
};

struct Position {
  int x, y;
};

struct TuringHead {
  uint32_t state;
  struct Command {
    uint8_t value;
    bool get_output() { return value & 1; }
    bool get_movement() { return (value >> 1) & ((1<<3)-1); }
    bool get_state() { return value >> 4; }
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
