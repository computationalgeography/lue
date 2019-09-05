import os.path
import sys
sys.path = [
    os.path.join(os.path.split(__file__)[0], "..")
] + sys.path
from lue.benchmark.pool import *
import unittest


class PoolTest(unittest.TestCase):

    def _test_case(self,
            min_size,
            max_size,
            multiplier,
            permutation_sizes):

        data = {
            "min_size": min_size,
            "max_size": max_size,
            "multiplier": multiplier,
        }

        pool = Pool(data)

        self.assertEqual(pool.nr_permutations(), len(permutation_sizes))
        self.assertEqual(pool.min_size, permutation_sizes[0])
        self.assertEqual(pool.max_size, permutation_sizes[-1])

        for i in range(len(permutation_sizes)):
            self.assertEqual(pool.permutation_size(i), permutation_sizes[i])

    def test_case_1_64_2(self):
        self._test_case(1, 64, 2, [1, 2, 4, 8, 16, 32, 64])

    def test_case_1_2_2(self):
        self._test_case(1, 2, 2, [1, 2])

    def test_case_1_3_2(self):
        self._test_case(1, 3, 2, [1, 2])

    def test_case_4_35_3(self):
        self._test_case(4, 35, 3, [4, 12])

    def test_case_4_36_3(self):
        self._test_case(4, 36, 3, [4, 12, 36])

    def test_case_4_37_3(self):
        self._test_case(4, 37, 3, [4, 12, 36])
