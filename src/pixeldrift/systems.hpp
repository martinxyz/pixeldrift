#pragma once
#include "components.hpp"
#include "tile.hpp"

void sensor_system_tick(struct World * world) {
  const int mask = (1 << kVision) | (1 << kPosition);
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (!(world->mask[i] & mask)) continue;
    int x = world->position[i].x;
    int y = world->position[i].y;
    auto &occ = world->vision[i].occupied;
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

void actor_system_tick(struct World * world) {
  const int mask = (1 << kTuringHead) | (1 << kPosition) | (1 << kVision);
  for (int i = 0; i < MAX_ENTITIES; i++) {
    if (!(world->mask[i] & mask)) continue;

    uint8_t current_state = world->turing_head[i].state;
    auto &occ = world->vision[i].occupied;

    uint16_t key = 0;
    for (int i=0; i<7; i++) {
      key |= (occ[i] & 1) << i;
    }
    key |= current_state << 7;

    TuringHead::Command cmd;
    cmd = world->turing_head[i].command_lut[key];

    int x = world->position[i].x;
    int y = world->position[i].y;
    world->at(x, y).particle = cmd.output;
    world->turing_head[i].state = cmd.state;

    auto delta = kDirectionToDelta[cmd.movement];
    // if (world->at(x+delta.dx, y+delta.dy).block != kBorder) {
    //   x += delta.dx;
    //   y += delta.dy;
    // }
    x = (x + delta.dx) & (1 - tile_size);
    y = (y + delta.dy) & (1 - tile_size);

    world->position[i].x += x;
    world->position[i].y += y;
  }
}

void world_tick(struct World * world) {
  sensor_system_tick(world);
  actor_system_tick(world);
}
