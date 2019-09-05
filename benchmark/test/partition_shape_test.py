import os.path
import sys
sys.path = [
    os.path.join(os.path.split(__file__)[0], "..")
] + sys.path
from lue.benchmark.shape import *
import unittest


class PartitionShapeTest(unittest.TestCase):

    def test_partition_shape_multipliers(self):

        shape = (10, 10)

        partition_shape = (1, 1)
        multipliers = partition_shape_multipliers(shape, partition_shape)
        self.assertEqual(multipliers, [10, 10])

        partition_shape = (10, 10)
        multipliers = partition_shape_multipliers(shape, partition_shape)
        self.assertEqual(multipliers, [1, 1])

        partition_shape = (5, 5)
        multipliers = partition_shape_multipliers(shape, partition_shape)
        self.assertEqual(multipliers, [2, 2])

        partition_shape = (2, 5)
        multipliers = partition_shape_multipliers(shape, partition_shape)
        self.assertEqual(multipliers, [5, 2])

        partition_shape = (5, 2)
        multipliers = partition_shape_multipliers(shape, partition_shape)
        self.assertEqual(multipliers, [2, 5])


    def test_ranges_of_partition_shape_multipliers(self):

        shape = (10, 10)

        min_partition_shape = (1, 1)
        max_partition_shape = (10, 10)
        multiplier_ranges = ranges_of_partition_shape_multipliers(
            shape, min_partition_shape, max_partition_shape)
        self.assertEqual(multiplier_ranges[0], range(10, 1, -1))
        self.assertEqual(multiplier_ranges[1], range(10, 1, -1))

        min_partition_shape = (2, 2)
        max_partition_shape = (5, 5)
        multiplier_ranges = ranges_of_partition_shape_multipliers(
            shape, min_partition_shape, max_partition_shape)
        self.assertEqual(multiplier_ranges[0], range(5, 2, -1))
        self.assertEqual(multiplier_ranges[1], range(5, 2, -1))

        min_partition_shape = (5, 5)
        max_partition_shape = (5, 5)
        multiplier_ranges = ranges_of_partition_shape_multipliers(
            shape, min_partition_shape, max_partition_shape)
        self.assertEqual(multiplier_ranges[0], range(2, 2, -1))
        self.assertEqual(multiplier_ranges[1], range(2, 2, -1))


    def test_ranges_of_shapes(self):

        min_shape = (1, 1)
        max_shape = (10, 10)
        step = 1
        ranges = shape_ranges(min_shape, max_shape, step)

        self.assertEqual(ranges[0], range(1, 11, step))
        self.assertEqual(ranges[1], range(1, 11, step))


    def test_partition_shapes(self):

        min_shape = (480, 480)
        max_shape = (500, 500)
        step = 10
        shapes = list(partition_shapes(min_shape, max_shape, step))

        self.assertEqual(len(shapes), 3)
        self.assertEqual(shapes[0], (480, 480))
        self.assertEqual(shapes[1], (490, 490))
        self.assertEqual(shapes[2], (500, 500))
