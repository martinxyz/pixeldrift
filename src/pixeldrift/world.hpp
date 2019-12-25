#pragma once
#include <cstdlib>
#include <cstring>
#include "components.hpp"

void sensor_system_tick(struct World * world);
void turing_head_system_tick(struct World * world);

constexpr int MAX_ENTITIES = 500;
struct World {
  int mask[MAX_ENTITIES];
  // ActionInput input[MAX_ENTITIES];
  Vision vision[MAX_ENTITIES];
  Position position[MAX_ENTITIES];
  TuringHead turing_head[MAX_ENTITIES];

  TileContent map[tile_size * tile_size]{};
  TileContent& at(int x, int y) {
    return map[y*tile_size + x];
  }

  World() {
    bzero(mask, sizeof(mask));
    bzero(map, sizeof(map));
  }
  void tick() {
    sensor_system_tick(this);
    turing_head_system_tick(this);
  }

  int first_unused_entity() {
    for (int ent = 0; ent < MAX_ENTITIES; ++ent) {
      if (mask[0] == 0) return ent;
    }
    abort();
  }

  /*
  bool add_turing_head(lut, state0) {
    for (int ent=0; ent<MAX_ENTITIES; i++) {
      if (mask[ent]) {
        mask[ent] = ...;
        return true;
      }
    }
    return false;
  }
  */
};
