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

        array_shape = (600, 400)
        partition_shape = (10, 10)

        direction = 2
        flow_direction = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint8), direction)
        material = lfr.create_array(array_shape, partition_shape, np.dtype(np.float64), 1)
        fraction = lfr.create_array(array_shape, partition_shape, np.dtype(np.float64), 0.8)

        flux, state = lfr.accu_fraction(flow_direction, material, fraction)
