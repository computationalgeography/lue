import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    pass
    # lue_test.stop_hpx_runtime()


class LocalityIDTest(lue_test.TestCase):

    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)

        for type_ in [np.uint8, np.uint32, np.uint64, np.int32, np.int64, np.float32, np.float64]:
            array = lfr.create_array(array_shape, partition_shape, np.dtype(type_), 0)
            locality_id = lfr.locality_id(array)
