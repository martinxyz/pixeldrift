#pragma once
#include <cstdlib>
#include <cstring>
#include <entt/entity/registry.hpp>
#include "components.hpp"

void sensor_system_tick(struct World * world);
void turing_head_system_tick(struct World * world);

struct World {
  entt::registry registry;

  TileContent map[tile_size * tile_size]{};
  TileContent& at(int x, int y) {
    x = static_cast<unsigned>(x) % tile_size;
    y = static_cast<unsigned>(y) % tile_size;
    return map[y*tile_size + x];
  }

  World() {
    bzero(map, sizeof(map));
  }

  void tick(int n=1) {
    for (int i = 0; i < n; ++i) {
      sensor_system_tick(this);
      turing_head_system_tick(this);
    }
  }
};
