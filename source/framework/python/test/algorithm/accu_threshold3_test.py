import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    pass
    # lue_test.stop_hpx_runtime()


class AccuThreshold3Test(lue_test.TestCase):

    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)

        for direction in [1, 2, 3, 4, 5, 6, 7, 8, 9]:
            flow_direction = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint8), direction)
            external_inflow = lfr.create_array(array_shape, partition_shape, np.dtype(np.float64), 1)
            threshold = lfr.create_array(array_shape, partition_shape, np.dtype(np.float64), 5)

            outflow, remainder = lfr.accu_threshold3(flow_direction, external_inflow, threshold)
