from .pixeldrift import World as _World
from .pixeldrift import CellType as _CellType
from .cells import Cells, CellContent
from typing import NamedTuple, List

class CellType(NamedTuple):
    child1: int
    child2: int
    child1_maxcount: int = 0

class World():  # to be changed... World(tiles)
    def __init__(self):
        # self.cells = cells
        self._world = _World()
        self.cells = Cells(_storage=self._world)
        self.managed_classes = set()

    def get_particles(self, x0=0, y0=0, w=None, h=None):
        w = w or 0
        h = h or 0
        return self._world.get_particles(x0, y0, w, h)

    def add_turing_head(self, turing_heads, x, y):
        turing_heads.add_instance(self._world, x, y)
        # keep this reference alive (so self._world can safely store pointers to it)
        self.managed_classes.add(turing_heads)

    def tick(self, count=1):
        return self._world.tick(count)

    def tick6(self):
        return self._world.tick6()

    def tick6_single(self, direction: int):
        return self._world.tick6_single(direction)

    def set_cell_types(self, cell_types: List[CellType]):
        # todo: sanity checks, names, etc. and possibly some encoding
        types = []
        for ct in cell_types:
            v = _CellType()
            # for name in CellType._fields:
            #     setattr(v, name, ct[name])
            v.child1 = ct.child1
            v.child2 = ct.child2
            v.child1_maxcount = ct.child1_maxcount
            types.append(v)
        self._world.cell_types = types

    def set_cell(self, x: int, y: int, data: CellContent):
        self.cells.set_cell(x, y, data)

    def get_cell(self, x, y) -> CellContent:
        return self.cells.get_cell(x, y)
