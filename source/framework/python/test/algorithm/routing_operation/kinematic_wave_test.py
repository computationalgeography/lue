import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class KinematicWaveTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        direction = 3
        alpha = 1.5
        beta = 0.6
        time_step_duration = 15

        flow_direction = lfr.create_array(array_shape, np.uint8, direction)

        for dtype in [np.float32, np.float64]:
            discharge = lfr.create_array(array_shape, dtype, 5)
            inflow = lfr.create_array(array_shape, dtype, 1)
            channel_length = lfr.create_array(array_shape, dtype, 10)

            lfr.kinematic_wave(
                flow_direction,
                discharge,
                inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length,
            )
