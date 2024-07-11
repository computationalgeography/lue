import numpy as np

import lue.framework_x as lfrx
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class CreateScalarTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_create_scalar(self):
        for element_type in [
            np.uint8,
            np.uint32,
            np.int32,
            np.uint64,
            np.int64,
            np.float32,
            np.float64,
        ]:
            lfrx.create_scalar(5, element_type)
