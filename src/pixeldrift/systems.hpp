#pragma once
#include "world.hpp"

void sensor_system_tick(struct World * world) {
  auto view = world->registry.view<Vision, Position>();
  for (auto entity: view) {
    auto &vision = view.get<Vision>(entity);
    auto &position = view.get<Position>(entity);
    int x = position.x;
    int y = position.y;
    auto &occ = vision.occupied;
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
