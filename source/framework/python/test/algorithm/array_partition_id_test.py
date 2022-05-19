import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ArrayPartitionIDTest(lue_test.TestCase):

    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)

        array = lfr.create_array(array_shape, partition_shape, np.dtype(np.uint8), fill_value=5)

        partition_id = lfr.array_partition_id(array)
