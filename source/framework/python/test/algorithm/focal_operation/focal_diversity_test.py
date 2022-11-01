import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class FocalDiversityTest(lue_test.TestCase):

    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        fill_value = 5
        kernel = np.full((3, 3), 1, dtype=np.uint8)

        for dtype in [np.uint8, np.int32, np.uint32, np.int64, np.uint64]:
            array = lfr.create_array(array_shape, partition_shape, np.dtype(dtype), fill_value)
            lfr.focal_diversity(array, kernel)
