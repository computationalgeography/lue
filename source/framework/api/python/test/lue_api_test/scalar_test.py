import numpy as np

import lue.framework_x as lfrx
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ScalarTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_scalar(self):

        for element_type in lfrx.arithmetic_element_types:
            pass
            # _ = lfrx.Scalar(5, element_type)
