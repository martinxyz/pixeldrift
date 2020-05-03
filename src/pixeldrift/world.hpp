#pragma once
#include <cstdlib>
#include <cstring>
#include <memory>
#include <random>
#include <algorithm>
#include <entt/entity/registry.hpp>
#include "components.hpp"

void sensor_system_tick(struct World * world);
void turing_head_system_tick(struct World * world);

struct World {
  entt::registry registry;
  TorusTileStore map;
  std::vector<CellType> cell_types;
  std::minstd_rand rng;

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

  inline bool probability(uint8_t p) {
    // if (p == 0) return false;
    // if (p >= 128) return true;
    return rng() % 128 < p;
  }

  struct Transaction {
    bool split{false};
    CellContent child1{};
    CellContent child2{};
  };

  inline void process_line(CellContent * p, int stride, int count) {
    CellContent prev = *(p - stride);
    CellContent cur = *(p);
    Transaction prev_to_cur = get_transaction(prev, cur);
    for (int i=0; i<count; i++) {
      CellContent next = *(p + stride);
      Transaction cur_to_next = get_transaction(cur, next);
      *p = execute_transactions(prev_to_cur, cur, cur_to_next);
      p += stride;
      prev = cur;
      cur = next;
      prev_to_cur = cur_to_next;
    }
  }

  inline Transaction get_transaction(CellContent cur, CellContent next) {
    CellType cur_ct = cell_types[cur.cell_type];
    if (probability(cur_ct.skip_transaction_p)) {
      return {};
    }

    if (next.cell_type == 0 &&
        cur.cell_type != 0 &&
        cur.child_count < cur_ct.child_maxcount) {
      CellContent child1 {
        .cell_type = cur.cell_type,
        .child_count = static_cast<uint8_t>(std::min(cur.child_count + 1, 255)),
      };
      CellContent child2 {
        .cell_type = cur_ct.child,
      };
      bool child1_first = probability(cur_ct.child_at_parent_location_p);
      return {
        .split = true,
        .child1 = child1_first ? child1 : child2,
        .child2 = child1_first ? child2 : child1,
      };
    }
    return {};
  }

  inline CellContent execute_transactions(Transaction prev_to_cur,
                                          CellContent cur,
                                          Transaction cur_to_next) {
    // prev creates child2
    if (prev_to_cur.split) {
      assert(!cur_to_next.split);
      return prev_to_cur.child2;
    }

    // cur creates child1
    if (cur_to_next.split) {
      return cur_to_next.child1;
    }

    // noop
    return cur;
  }

};
