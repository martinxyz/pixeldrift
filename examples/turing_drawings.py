#!/usr/bin/env python3
import numpy as np
import time
from numpy.random import randint
import pixeldrift
from pixeldrift import World, TuringHeads

def random_lut():
  input_bits = 7
  state_bits = 4
  size = 2**(input_bits+state_bits)
  next_output = randint(0, 2, size)
  # next_output = np.ones(size, dtype='uint8')
  # next_movement = np.ones(size, dtype='uint8') * d
  next_movement = randint(0, 6, size)
  next_state = randint(0, 2**4, size)
  # next_state = np.ones(size, dtype='uint8') * 0xFF


  # bool get_movement() { return (value >> 1) & ((1<<3)-1); }
  return (next_output | (next_movement << 1) | (next_state << 4)).astype('uint8')


t0 = time.time()
for j in range(100):
  w = World()

  th = TuringHeads()
  th.add_head(w, pixeldrift.tile_size//2, pixeldrift.tile_size//2)

  th.set_lut(random_lut())
  for i in range(200_000):
    w.tick(100)
  pixeldrift.render(w.get_particles(), f'output_{j:04}.png')
print('time.time() - t0:', time.time() - t0)

