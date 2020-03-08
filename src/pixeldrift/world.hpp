#pragma once
#include <cstdlib>
#include <cstring>
#include <memory>
#include <entt/entity/registry.hpp>
#include "components.hpp"

void sensor_system_tick(struct World * world);
void turing_head_system_tick(struct World * world);

struct World {
  entt::registry registry;
  TorusTileStore map;
  std::vector<CellType> cell_types;

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
      CellContent * old_data = old_tile.data_inside();
      CellContent * new_data = tile->data_inside();
      for (int y=0; y<Tile::size; y++) {
        int x = 0;
        for (int x_=0; x_<Tile::size/4; x_++) {
          for (int x2=0; x2<4; x2++) {  // helps gcc with unrolling (~20% speedup)
            int key = 0;
            for (int i=0; i<7; i++) {
              auto delta = kDirectionToDelta[i];
              CellContent &tc = old_data[(y+delta.dy)*Tile::stride_y + x+delta.dx];
              key |= tc.particle << i;
            }
            new_data[y*Tile::stride_y + x].particle = lut[key] & 1;
            x++;
          }
        }
      }
    }
  }

  double count_lut_filter(uint8_t * lut) {
    map.update_borders();
    uint32_t count = 0;
    uint32_t total = 0;
    for (Tile* tile: map.iter_tiles()) {
      Tile old_tile{*tile};
      CellContent * old_data = old_tile.data_inside();
      for (int y=0; y<Tile::size; y++) {
        int x = 0;
        for (int x_=0; x_<Tile::size/4; x_++) {
          for (int x2=0; x2<4; x2++) {  // helps gcc with unrolling (~20% speedup)
            int key = 0;
            for (int i=0; i<7; i++) {
              auto delta = kDirectionToDelta[i];
              CellContent &tc = old_data[(y+delta.dy)*Tile::stride_y + x+delta.dx];
              key |= tc.particle << i;
            }
            count += lut[key] & 1;
            x++;
          }
        }
      }
      total += Tile::size * Tile::size;
    }
    return static_cast<double>(count) / static_cast<double>(total);
  }


  void tick6() {
    // TODO: order to be shuffled
    std::vector<int> order{0, 1, 2, 3, 4, 5};
    tick6_internal(order);
  }

  // for easy testing
  void tick6_single(int direction) {
    assert(direction >= 0 && direction < 6);
    std::vector<int> order{direction};
    tick6_internal(order);
  }

 private:

  void tick6_internal(const std::vector<int> &order) {
    map.update_borders();
    for (Tile* tile: map.iter_tiles()) {
      for (int direction: order) {
        constexpr auto size = Tile::size;
        constexpr auto stride_y = Tile::stride_y;
        CellContent * const p = tile->data_inside();
        switch (direction) {
          case 0:  // {+1, 0}
            for (int y=0; y<size; y++) {
              process_line(p + y*stride_y, 1, size);
            }
            break;
          case 3:  // {-1, 0}
            for (int y=0; y<size; y++) {
              process_line(p + y*stride_y + (size - 1), -1, size);
            }
            break;
          case 1:  // { 0,+1}
            for (int x=0; x<size; x++) {
              process_line(p + x, stride_y, size);
            }
            break;
          case 4:  // { 0,-1}
            for (int x=0; x<size; x++) {
              process_line(p + (size - 1) * stride_y + x, -stride_y, size);
            }
            break;
          case 5: {  // {+1,-1}
            constexpr int stride = 1 - stride_y;
            for (int y=0; y<size; y++) {
              int count = y+1;
              process_line(p + y*stride_y, stride, count);
            }
            for (int x=1; x<size; x++) {
              int count = size - x;
              process_line(p + size*stride_y + x, stride, count);
            }
          } break;
          case 2: {  // {-1,+1}
            constexpr int stride = - 1 + stride_y;
            for (int y=0; y<size; y++) {
              int count = y+1;
              process_line(p + y*stride_y - stride*(count-1), stride, count);
            }
            for (int x=1; x<size; x++) {
              int count = size - x;
              process_line(p + size*stride_y + x - stride*(count-1), stride, count);
            }
          } break;
          default:
            abort();
        }
      }
    }
  }

  inline void process_line(CellContent * p, int stride, int count) {
    CellContent prev = *(p - stride);
    CellContent cur = *(p);
    for (int i=0; i<count; i++) {
      CellContent next = *(p + stride);
      *p = process_one(prev, cur, next);
      p += stride;
      prev = cur;
      cur = next;
    }
  }

  inline CellContent process_one(CellContent prev, CellContent cur, CellContent next) {
    if (cur.cell_type != 0 && next.cell_type == 0) {
      // block at current location can split, and creates child1 here
      CellType ct = cell_types[cur.cell_type];
      return {
        .cell_type = ct.child1
      };
      // res.child1_count = blockClasses[cur.cell_type].child1;
    }
    if (cur.cell_type == 0) {
      // block at previous location creates child2 here
      CellType ct = cell_types[prev.cell_type];
      return {
        .cell_type = ct.child2
      };
    }
    // current location does not change its blockClasses
    return cur;
  }

};
