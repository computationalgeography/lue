import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class FocalSumTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5
        kernel = np.full((3, 3), 1, dtype=np.uint8)

        for dtype in [
            np.uint8,
            np.int32,
            np.uint32,
            np.int64,
            np.uint64,
            np.float32,
            np.float64,
        ]:
            array = lfr.create_array(array_shape, dtype, fill_value)
            lfr.focal_sum(array, kernel)
