import unittest
import numpy as np
import pixeldrift

class MainTest(unittest.TestCase):
    def test_f(self):
        data = np.zeros(9, dtype='uint8')
        data[5] = 41
        res = pixeldrift.f(data)
        print('res:', res)
        # self.assertEqual(data[5], 42)
        self.assertEqual(data[5], str(res))

if __name__ == '__main__':
    unittest.main()
