import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class AspectTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)

        fill_value = 5
        elevation = lfr.create_array(array_shape, np.float32, fill_value)

        aspect = lfr.aspect(elevation)
