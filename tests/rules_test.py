import unittest
import numpy as np
import pixeldrift

class MainTest(unittest.TestCase):
    def test_f(self):
        data = np.zeros(4, dtype='uint8')
        res = pixeldrift.f(data)
        self.assertEqual(res, 2)

if __name__ == '__main__':
    unittest.main()
