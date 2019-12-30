from .pixeldrift import World as _World
from .cells import Cells

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
