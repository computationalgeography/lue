import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class LogTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.float64
        fill_value = 5.5
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)

        lfr.log(array)
