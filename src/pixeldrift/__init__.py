from .pixeldrift import World as _World
from .pixeldrift import TuringHeads, tile_size
# from .hello import say_hello
from .render import render

class World():
    def __init__(self):
        self._world = _World()

    def get_particles(self, x0=0, y0=0, w=None, h=None):
        w = w or 0
        h = h or 0
        return self._world.get_particles(x0, y0, w, h)

    def tick(self, count=1):
        return self._world.tick(count)
