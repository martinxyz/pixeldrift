import unittest
import numpy as np
from numpy.testing import assert_array_equal
from pixeldrift import Cells, tile_size

class CellTest(unittest.TestCase):
    def test_initialization(self):
        cells = Cells()
        self.assertTrue(np.all(cells.get_data() == 0))

    def test_mask(self):
        cells = Cells()
        self.assertTrue(np.all(cells.get_mask()))
        self.assertEqual(np.sum(cells.get_mask(-1000, 0, w=1000)), 0)
        self.assertEqual(np.sum(cells.get_mask(0, -1000, h=1000)), 0)
        self.assertEqual(np.sum(cells.get_mask(-999, 0, w=1000, h=1)), 1)
        self.assertEqual(np.sum(cells.get_mask(0, -999, w=1, h=1000)), 1)
        self.assertEqual(np.sum(cells.get_mask(tile_size-1, 0, w=1000, h=7)), 7)
        self.assertEqual(np.sum(cells.get_mask(0, tile_size-1, w=7, h=1000)), 7)

    def test_data_tiling(self):
        cells = Cells()
        data = np.random.randint(0, 2, size=(tile_size, tile_size))
        for dx, dy in [(0, 0), (-tile_size, 0), (0, tile_size), (-7*tile_size, -9*tile_size)]:
            msg = f'dx,dy: {dx, dy}'
            cells.set_data(data, x=dx, y=dy)
            assert_array_equal(cells.get_data(), data, msg)
            assert_array_equal(cells.get_data(x=tile_size), data, msg)
            assert_array_equal(cells.get_data(x=-tile_size, y=-30*tile_size), data, msg)

        cells.set_data(np.zeros((tile_size, tile_size)), -33, -11)
        self.assertTrue(np.all(cells.get_data() == 0))
        self.assertTrue(np.all(cells.get_data(-88, -88, tile_size+2*88, tile_size+2*88) == 0))

    def test_data_offset_coordinates(self):
        cells = Cells()
        data = np.ones((2, 7), dtype='uint8')
        cells.set_data(data, -1, -1)
        self.assertEqual(np.sum(cells.get_data()), 2*7)
        self.assertEqual(np.sum(cells.get_data(-1, -1, 7, 2)), 2*7)
        self.assertEqual(np.sum(cells.get_data(0, 0, 20, 20)), 6)
        self.assertEqual(np.sum(cells.get_data(-1, 0, 20, 20)), 7)
        self.assertEqual(np.sum(cells.get_data(0, -1, 20, 20)), 2*6)


if __name__ == '__main__':
    unittest.main()
