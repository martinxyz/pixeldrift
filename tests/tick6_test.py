import unittest
import numpy as np
from numpy.testing import assert_array_equal
from pixeldrift import Cells, World, tile_size, CellType, CellContent

class Tick6Test(unittest.TestCase):
    def test_basics(self):
        w = World()
        w.set_cell_types([
            CellType(0, 0, 255),
            CellType(0, 0, 255),
        ])
        self.assertEqual(w.get_cell(30, 30).cell_type, 0)
        w.set_cell(30, 30, CellContent(
            cell_type=1,
            child1_count=0,
            particle=0,
        ))
        self.assertEqual(w.get_cell(30, 30).cell_type, 1)

    def test_simple_cell(self):
        for direction in range(6):
            with self.subTest(direction=direction):
                w = World()
                w.set_cell_types([
                    CellType(0, 0, 255),
                    CellType(2, 1, 255),
                    CellType(0, 0, 255),
                ])
                w.set_cell(30, 30, CellContent(
                    cell_type=1,
                    child1_count=0,
                    particle=0,
                ))

                w.tick6_single(direction)

                self.assertEqual(w.get_cell(30, 30).cell_type, 2)
                self.assertEqual(w.cells.count_cells_of_type(2), 1)
                self.assertEqual(w.cells.count_cells_of_type(1), 1)

                w.tick6_single(direction)

                self.assertEqual(w.get_cell(30, 30).cell_type, 2)
                self.assertEqual(w.cells.count_cells_of_type(2), 2)
                self.assertEqual(w.cells.count_cells_of_type(1), 1)

    def test_moving_cell(self):
        for direction in range(6):
            with self.subTest(direction=direction):
                w = World()
                w.set_cell_types([
                    CellType(0, 0, 255),
                    CellType(0, 1, 255),
                ])
                w.set_cell(30, 30, CellContent(
                    cell_type=1,
                    child1_count=0,
                    particle=0,
                ))

                self.assertEqual(w.get_cell(30, 30).cell_type, 1)
                w.tick6_single(direction)
                self.assertEqual(w.get_cell(30, 30).cell_type, 0)
                self.assertEqual(w.cells.count_cells_of_type(1), 1)

                for i in range(2*tile_size):
                    w.tick6_single(direction)
                self.assertEqual(w.cells.count_cells_of_type(1), 1)


if __name__ == '__main__':
    unittest.main()
