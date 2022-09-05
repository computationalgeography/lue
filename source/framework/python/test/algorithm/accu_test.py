import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class AccuTest(lue_test.TestCase):

    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)

        direction = 4
        flow_direction = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint8), direction)
        material = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint64), 1)

        flux = lfr.accu(flow_direction, material)
