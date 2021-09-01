import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    pass
    # lue_test.stop_hpx_runtime()


class ToNumPyTest(lue_test.TestCase):

    def test_array(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.dtype(np.int32)
        fill_value = 5
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)
        numpy_array = lfr.to_numpy(array)

        self.assertEqual(numpy_array.dtype, dtype)
        np.testing.assert_array_equal(numpy_array, np.full(array_shape, fill_value, dtype=dtype))


    def test_empty_array(self):

        # TODO Crashes...
        pass

        # array_shape = (1, 1)
        # partition_shape = (1, 1)
        # dtype = np.dtype(np.int32)
        # fill_value = 5
        # array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)
        # numpy_array = lfr.to_numpy(array)

        # self.assertEqual(numpy_array.dtype, dtype)
        # np.testing.assert_array_equal(numpy_array, np.full(array_shape, fill_value, dtype=dtype))
