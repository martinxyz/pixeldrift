from .pixeldrift import World as _World
from .pixeldrift import tile_size
import numpy as np

class Cells():
    """Represents a hexagonal grid of cells.

    Currently only a hardcoded tile_size with toroidal tiling is supported.

    The API is designed to be extended for other geometries. (At least fixed
    borders of any shape without tiling; maybe also hexagonal mirroring.)
    """

    def __init__(self, _storage=None):
        self._cells = _storage or _World()

    def get_bbox(self):
        """Get a minimal bounding box that includes all core cells"""
        return (0, 0, tile_size, tile_size)

    def get_data(self, x=0, y=0, w=tile_size, h=tile_size):
        """Get a copy of the cell data in a region.

        Returns a new numpy array of type 'uint8' and shape (h, w).

        The region may be smaller or larger than one tile. The returned array
        uses offset coordinates. If y is even the array will be odd-rows-right,
        otherwise odd-rows-left.
        """
        return self._cells.get_particles(x, y, w, h)

    def set_data(self, data, x=0, y=0):
        """Set the cell data of a region.

        Data must be a numpy array of type 'uint8' and can have any 2d-shape.
        It is possible to write into mirrored locations, and to overwrite the
        same core cell multiple times. The last written value will have effect.

        Same offset coordinates apply as for get_data().
        """
        return self._cells.set_particles(data.astype('uint8'), x, y)

    def get_mask(self, x=0, y=0, w=tile_size, h=tile_size):
        """Get the bitmask for core cells in a region.

        Returns a new numpy array of type 'bool' and shape (h, w).

        The mask is False for borders (if used) and for any tiled or mirrored
        copies of core cells.
        """
        res = np.zeros((h, w), dtype='bool')
        x0, y0 = max(-x, 0), max(-y, 0)
        x1, y1 = min(-x + tile_size, w), min(-y + tile_size, h)
        if x1 > x0 and y1 > y0:
            res[y0:y1, x0:x1] = True
        return res
