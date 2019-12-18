from .pixeldrift import DiffusionMap, f
import numpy as np

def say_hello():
    print("Hello world!")
    m = DiffusionMap()
    print('m.arr:', m.arr)
    data = np.zeros(4, dtype='uint8')
    print('data.dtype:', data.dtype)
    print('f(data) -->', f(data))

say_hello()
