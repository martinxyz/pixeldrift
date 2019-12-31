#!/usr/bin/env python3
import time
from numpy.random import randint
from pixeldrift import World, TuringHeads, tile_size, render_cells
import subprocess


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

outputs = []
for k in range(10):
    w = World()
    th = TuringHeads()
    w.add_turing_head(th, tile_size//2, tile_size//2)

    t0 = time.time()
    for j in range(5):
        th.set_lut(random_lut())
        w.tick(100_000)

    print(f'turing drawing {k} created in {(time.time()-t0)*1000:.1f}ms')
    filename = f'output_{k:04}.png'
    render_cells(w.cells, filename, padding=5, grid=False)
    outputs.append(filename)

print('startig image viewer "feh"')
subprocess.call(['feh'] + outputs)
