#pragma once
#include "world.hpp"

void sensor_system_tick(struct World * world) {
  const int mask = (1 << kVision) | (1 << kPosition);
  for (int ent = 0; ent < MAX_ENTITIES; ent++) {
    if (!(world->mask[ent] & mask)) continue;
    int x = world->position[ent].x;
    int y = world->position[ent].y;
    auto &occ = world->vision[ent].occupied;
    auto get = [world, x, y](int dx, int dy) {
      return world->at(x+dx, y+dy).particle;
    };
    occ[0] = get( 0, -1);
    occ[1] = get(-1, -1);
    occ[2] = get(-1,  0);
    occ[3] = get( 0,  0);
    occ[4] = get(+1,  0);
    occ[5] = get(-1, +1);
    occ[6] = get( 0, +1);
  }
}
