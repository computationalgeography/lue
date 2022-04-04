import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class FromNumPyTest(lue_test.TestCase):

    def test_array(self):

        array_shape = (60, 40)
        nr_cells = 60 * 40
        partition_shape = (10, 10)
        dtype = np.dtype(np.int32)
        numpy_array = np.arange(nr_cells, dtype=dtype).reshape(array_shape)
        lue_array = lfr.from_numpy(numpy_array, partition_shape)

        self.assertEqual(lue_array.dtype, dtype)
        self.assertEqual(lue_array.shape, array_shape)
        self.assertEqual(lfr.minimum(lue_array).get(), 0)
        self.assertEqual(lfr.maximum(lue_array).get(), nr_cells - 1)
