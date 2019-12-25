#pragma once
#include <cstdlib>
#include <vector>
#include "world.hpp"

void turing_head_system_tick(struct World * world) {
  const int mask = (1 << kTuringHead) | (1 << kPosition) | (1 << kVision);
  for (int ent = 0; ent < MAX_ENTITIES; ent++) {
    if (!(world->mask[ent] & mask)) continue;

    uint8_t current_state = world->turing_head[ent].state;
    auto &occ = world->vision[ent].occupied;

    uint16_t key = 0;
    for (int i=0; i<7; i++) {
      key |= (occ[i] & 1) << i;
    }
    key |= (current_state & 0xF) << 7;

    TuringHead::Command cmd;
    cmd = world->turing_head[ent].command_lut[key];

    int x = world->position[ent].x;
    int y = world->position[ent].y;
    world->at(x, y).particle = cmd.get_output();
    world->turing_head[ent].state = cmd.get_state();

    auto delta = kDirectionToDelta[cmd.get_movement()];
    // if (world->at(x+delta.dx, y+delta.dy).block != kBorder) {
    //   x += delta.dx;
    //   y += delta.dy;
    // }
    x = (x + delta.dx) & (1 - tile_size);
    y = (y + delta.dy) & (1 - tile_size);

    world->position[ent].x += x;
    world->position[ent].y += y;
  }
}

class TuringHeads
{
 public:
  constexpr static int lut_size = 1 << (7 + 4);

 private:
  TuringHead::Command lut_[lut_size]{0};
  std::vector<int> managed_entities_;

 public:
  void add_head(World *w, int pos_x, int pos_y) {
    int ent = w->first_unused_entity();

    w->turing_head[ent].command_lut = lut_;
    w->turing_head[ent].state = 0;
    w->mask[ent] |= 1 << kTuringHead;

    w->position[ent] = {pos_x, pos_y};
    w->mask[ent] |= 1 << kPosition;

    w->vision[ent] = {};
    w->mask[ent] |= 1 << kVision;
  }

  void set_lut(void * data) {
    memcpy(lut_, data, sizeof(lut_));
  }
};
