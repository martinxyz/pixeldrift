#!/usr/bin/env python3
import numpy as np
from numpy.random import randint
from pixeldrift import World, TuringHeads

def random_lut():
  input_bits = 7
  state_bits = 4
  size = 2**(input_bits+state_bits)
  next_output = randint(0, 2, size)
  next_movement = randint(0, 6, size)
  next_state = randint(0, 2**4, size)
  return (next_output | (next_movement << 3) | next_state << 4).astype('uint8')

w = World()
print('w.get_particles():', w.get_particles())
w.tick()

th = TuringHeads()
th.set_lut(random_lut())
th.add_head(w, 20, 20)

for j in range(10):
  for i in range(30):
    w.tick()
  th.set_lut(random_lut())

print('w.get_particles():', w.get_particles().sum())
