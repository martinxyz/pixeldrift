#pragma once
#include <cstdlib>
#include <vector>
#include "world.hpp"

void turing_head_system_tick(struct World * world) {
  world->registry.view<TuringHead, Vision, Position>()
  .each([world](auto &turing_head, auto &vision, auto &position) {
    uint8_t current_state = turing_head.state;
    auto &occ = vision.occupied;

    uint16_t key = 0;
    for (int i=0; i<7; i++) {
      key |= (occ[i] & 1) << i;
    }
    key |= (current_state & 0xF) << 7;

    TuringHead::Command cmd;
    cmd = turing_head.command_lut[key];

    int x = position.x;
    int y = position.y;
    world->map.at(x, y).particle = cmd.get_output();
    turing_head.state = cmd.get_state();

    auto delta = kDirectionToDelta[cmd.get_movement()];
    // if (world->at(x+delta.dx, y+delta.dy).cell_type != kBorder) {
    //   x += delta.dx;
    //   y += delta.dy;
    // }
    x += delta.dx;
    y += delta.dy;

    position.x = x;
    position.y = y;
  });
}

class TuringHeads
{
 public:
  constexpr static int lut_size = 1 << (7 + 4);

 private:
  TuringHead::Command lut_[lut_size]{{0}};
  std::vector<int> managed_entities_;

 public:
  void add_instance(World *w, int pos_x, int pos_y) {
    auto entity = w->registry.create();
    auto &turing_head = w->registry.assign<TuringHead>(entity);
    turing_head.command_lut = lut_;
    turing_head.state = 0;

    w->registry.assign<Position>(entity) = {pos_x, pos_y};
    w->registry.assign<Vision>(entity);
  }

  void set_lut(void * data) {
    memcpy(lut_, data, sizeof(lut_));
  }
};
