import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class D8FlowDirectionTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        direction = 6

        for type_ in [np.float32, np.float64]:
            elevation = lfr.create_array(array_shape, type_, direction)
            flow_direction = lfr.d8_flow_direction(elevation)

    @lue_test.framework_test_case
    def test_directions(self):
        array_shape = (60, 40)

        for direction in [1, 2, 3, 4, 5, 6, 7, 8, 9]:
            elevation = lfr.create_array(array_shape, np.float32, direction)
            flow_direction = lfr.d8_flow_direction(elevation)
