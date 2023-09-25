import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ArrayPartitionIDTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)

        array = lfr.create_array(array_shape, np.uint8, fill_value=5)

        partition_id = lfr.array_partition_id(array)
