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

  void apply_lut_filter(uint8_t * lut) {
    map.update_borders();
    for (Tile* tile: map.iter_tiles()) {
      Tile old_tile{*tile};
      TileContent * old_data = old_tile.data_inside();
      TileContent * new_data = tile->data_inside();
      for (int y=0; y<Tile::size; y++) {
        for (int x=0; x<Tile::size; x++) {
          int key = 0;
          for (int i=0; i<7; i++) {
            auto delta = kDirectionToDelta[i];
            TileContent &tc = old_data[(y+delta.dy)*Tile::stride_y + x+delta.dx];
            key |= tc.particle << i;
          }
          new_data[y*Tile::stride_y + x].particle = lut[key] & 1;
        }
      }
    }
  }
};
