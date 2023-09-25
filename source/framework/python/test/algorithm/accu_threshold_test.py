import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class AccuThresholdTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)

        for direction in [1, 2, 3, 4, 5, 6, 7, 8, 9]:
            flow_direction = lfr.create_array(array_shape, np.uint8, direction)
            material = lfr.create_array(array_shape, np.float64, 1)
            threshold = lfr.create_array(array_shape, np.float64, 5)
            flux, state = lfr.accu_threshold(flow_direction, material, threshold)
