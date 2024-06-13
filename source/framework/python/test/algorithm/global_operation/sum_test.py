import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class SumTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5

        for value_dtype in [
            np.uint8,
            np.uint32,
            np.uint64,
            np.int32,
            np.int64,
            np.float32,
            np.float64,
        ]:
            array = lfr.create_array(array_shape, value_dtype, fill_value)
            lfr.sum(array)
