import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class FocalMeanTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5
        kernel = np.full((3, 3), 1, dtype=lfr.boolean_element_type)

        for element_type in lfr.floating_point_element_types:
            array = lfr.create_array(array_shape, element_type, fill_value)
            lfr.focal_mean(array, kernel)
