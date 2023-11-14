import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class CreateArrayTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_create_array01(self):
        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.int32
        fill_value = 5

        # Pass tuple as partition shape
        array = lfr.create_array(
            array_shape, dtype, fill_value, partition_shape=partition_shape
        )

        self.assertEqual(array.dtype, dtype)
        self.assertEqual(array.shape, array_shape)
        self.assertTrue(lfr.all(array == fill_value).get())

    @lue_test.framework_test_case
    def test_create_array02(self):
        array_shape = (60, 40)
        dtype = np.int32
        fill_value = 5

        # Don't pass partition shape. Rely on the default partition shape.
        array = lfr.create_array(array_shape, dtype, fill_value)

        self.assertEqual(array.dtype, dtype)
        # self.assertEqual(array.shape, (?, ?))
        self.assertTrue(lfr.all(array == fill_value).get())
