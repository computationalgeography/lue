import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    pass
    # lue_test.stop_hpx_runtime()


class UniformTest(lue_test.TestCase):

    def test_overload(self):

        # TODO(KDJ) Support this overload
        pass
        # array_shape = (60, 40)
        # partition_shape = (10, 10)
        # dtype = np.dtype(np.int32)
        # min_value = 33
        # max_value = 55
        # array = lfr.uniform(array_shape, partition_shape, dtype, min_value, max_value)

        # self.assertEqual(array.dtype, dtype)
        # self.assertEqual(array.shape, array_shape)
        # self.assertTrue(lfr.all(min_value >= array <= max_value).get())
