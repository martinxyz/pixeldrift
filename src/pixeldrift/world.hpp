#pragma once
#include <cstdlib>
#include <cstring>
#include <entt/entity/registry.hpp>
#include "components.hpp"

void sensor_system_tick(struct World * world);
void turing_head_system_tick(struct World * world);

struct World {
  entt::registry registry;
  TorusTileStore map;

  void tick(int n=1) {
    for (int i = 0; i < n; ++i) {
      sensor_system_tick(this);
      turing_head_system_tick(this);
    }
  }
};
