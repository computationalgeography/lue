import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    pass
    # lue_test.stop_hpx_runtime()


class WaitTest(lue_test.TestCase):

    def test_overloads(self):
        array_shape = (60, 40)
        partition_shape = (10, 10)

        array = lfr.create_array(array_shape, partition_shape, np.dtype(np.int32), 5)
        lfr.wait(array)
