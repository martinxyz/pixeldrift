#!/usr/bin/env python3
from pixeldrift import World, CellType, CellContent, render_cells, tile_size
import random

w = World()

# Very loosely based on Zupanc et al., 2019: "Stochastic cellular automata model
# of tumorous neurosphere growth: Roles of developmental maturity and cell death"
w.set_cell_types([
    #        + child
    #        |
    #        |  + child_maxcount
    #        |  |
    CellType(0, 255),  # 0: empty
    CellType(2, 255),  # 1: stem cell
    CellType(3, 7),    # 2: progenitor cell
    CellType(0, 15),   # 3: differentiated cell (child_maxcount = max move dist)
])

# place a few stem cells
for i in range(10):
    x = random.randrange(tile_size)
    y = random.randrange(tile_size)
    w.set_cell(x, y, CellContent(cell_type=1, child_count=0, particle=0))

for k in range(1*20):
    filename = f'outputs/progenitor_{k:04}.png'
    render_cells(w.cells, filename, padding=5, grid=False)

    # w.tick6()
    w.tick6_single(k%6)
    # w.tick6_single(random.randrange(6))
