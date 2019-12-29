#!/usr/bin/env python3
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
    res = next_output | (next_movement << 1) | (next_state << 4)
    return res.astype('uint8')

w = World()
th = TuringHeads()
th.add_head(w._world, pixeldrift.tile_size//2, pixeldrift.tile_size//2)
t0 = time.time()
for j in range(50):
    th.set_lut(random_lut())
    for i in range(10):
        w.tick(10000)
print('time.time() - t0:', time.time() - t0)
# pixeldrift.render(w.get_particles(), f'output_{j:04}.png')
pixeldrift.render(w.get_particles(), f'output.png')
