import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class TanTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5.5

        for dtype in [np.float32, np.float64]:
            array = lfr.create_array(array_shape, dtype, fill_value)
            lfr.tan(array)
