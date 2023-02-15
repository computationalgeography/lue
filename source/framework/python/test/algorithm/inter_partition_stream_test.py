import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class InterPartitionStreamTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)

        direction = 4
        flow_direction = lfr.create_array(
            array_shape, partition_shape, np.uint8, direction
        )

        inter_partition_stream = lfr.inter_partition_stream(flow_direction)
