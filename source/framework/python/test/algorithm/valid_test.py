import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ValidTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_all_valid(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.int32
        fill_value = 5  # Valid value
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)

        result = lfr.valid(array)

        self.assertTrue(lfr.all(result == 1).get())

    @lue_test.framework_test_case
    def test_all_invalid(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.dtype(np.int32)
        fill_value = 5  # Valid value
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)

        # Turn all valid values into invalid ones
        array = lfr.where(array != fill_value, 7)

        result = lfr.valid(array)

        self.assertTrue(lfr.all(result == 0).get())
