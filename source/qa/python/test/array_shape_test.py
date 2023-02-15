import os.path
import sys


sys.path = [os.path.join(os.path.split(__file__)[0], "..")] + sys.path
import unittest

from lue.benchmark import *


class ArrayShapeTest(unittest.TestCase):
    def test_array_shape(self):

        memory = 100
        rank = 1
        nr_arrays = 1
        size_of_element = 1
        nr_nodes = 1

        shape = array_shape(memory, rank, nr_arrays, size_of_element, nr_nodes)
        self.assertEqual(shape, (100,))

    def test_array_shape_rank(self):

        memory = 100
        rank = 2
        nr_arrays = 1
        size_of_element = 1
        nr_nodes = 1

        shape = array_shape(memory, rank, nr_arrays, size_of_element, nr_nodes)
        self.assertEqual(shape, (10, 10))

    def test_array_shape_nr_arrays(self):

        memory = 100
        rank = 1
        nr_arrays = 2
        size_of_element = 1
        nr_nodes = 1

        shape = array_shape(memory, rank, nr_arrays, size_of_element, nr_nodes)
        self.assertEqual(shape, (50,))

    def test_array_shape_size_of_element(self):

        memory = 100
        rank = 1
        nr_arrays = 1
        size_of_element = 2
        nr_nodes = 1

        shape = array_shape(memory, rank, nr_arrays, size_of_element, nr_nodes)
        self.assertEqual(shape, (50,))
