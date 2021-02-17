import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class CreateArrayTest(lue_test.TestCase):

    def test_create_array(self):

        array_shape = (600, 400)
        partition_shape = (10, 10)
        dtype = np.dtype(np.int32)
        fill_value = 5
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)

        self.assertEqual(array.dtype, dtype)
        self.assertEqual(array.shape, array_shape)
        self.assertTrue(lfr.all(array == fill_value).get())
