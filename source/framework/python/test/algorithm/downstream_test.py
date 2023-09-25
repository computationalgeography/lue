import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class DownstreamTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)

        direction = 1
        flow_direction = lfr.create_array(array_shape, np.uint8, direction)

        for type_ in [
            np.uint8,
            np.int32,
            np.uint32,
            np.int64,
            np.uint64,
            np.float32,
            np.float64,
        ]:
            material = lfr.create_array(array_shape, type_, 1)
            lfr.downstream(flow_direction, material)
