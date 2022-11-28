import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class KinematicWaveTest(lue_test.TestCase):

    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        direction = 3
        alpha = 1.5
        beta = 0.6
        time_step_duration = 15
        channel_length = 10

        flow_direction = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint8), direction)

        for dtype in [np.float32, np.float64]:
            discharge = lfr.create_array(array_shape, partition_shape, np.dtype(dtype), 5)
            inflow = lfr.create_array(array_shape, partition_shape, np.dtype(dtype), 1)

            lfr.kinematic(flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length)
