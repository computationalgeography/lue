import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class Accu3Test(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)

        for direction in [1, 2, 3, 4, 5, 6, 7, 8, 9]:
            flow_direction = lfr.create_array(
                array_shape, partition_shape, np.uint8, direction
            )
            external_inflow = lfr.create_array(
                array_shape, partition_shape, np.float64, 1
            )

            outflow = lfr.accu3(flow_direction, external_inflow)
