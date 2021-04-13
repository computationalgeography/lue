import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    pass
    # lue_test.stop_hpx_runtime()


class AccuFractionTest(lue_test.TestCase):

    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)

        direction = 7
        flow_direction = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint8), direction)
        material = lfr.create_array(array_shape, partition_shape, np.dtype(np.float64), 1)
        threshold = lfr.create_array(array_shape, partition_shape, np.dtype(np.float64), 5)

        flux, state = lfr.accu_threshold(flow_direction, material, threshold)
