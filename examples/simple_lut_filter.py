#!/usr/bin/env python3
import numpy as np
from pixeldrift import Cells, tile_size, render_cells

cells = Cells()
cells.set_data(np.random.random((tile_size, tile_size)) < 0.01)

def make_directional_dilate_lut(neighbour):
    neighbour_mask = 1 << neighbour
    center_mask = 1 << 6
    lut = np.zeros(2**7, dtype='uint8')
    for key in range(2**7):
        if bool(key & (neighbour_mask | center_mask)):
            lut[key] = 1
    return lut

for n in range(6):
    for j in range(7):
        render_cells(cells, f'output-lut-filter-step{n:03}-{j:03}.png')
        cells.apply_lut_filter(make_directional_dilate_lut(n))
