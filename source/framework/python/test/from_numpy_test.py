import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class FromNumPyTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_array(self):

        array_shape = (60, 40)
        nr_cells = 60 * 40
        partition_shape = (10, 10)
        dtype = np.int32
        numpy_array = np.arange(nr_cells, dtype=dtype).reshape(array_shape)

        lue_array = lfr.from_numpy(numpy_array, partition_shape=partition_shape)

        self.assertEqual(lue_array.dtype, dtype)
        self.assertEqual(lue_array.shape, array_shape)
        self.assertEqual(lfr.minimum(lue_array).get(), 0)
        self.assertEqual(lfr.maximum(lue_array).get(), nr_cells - 1)

    @lue_test.framework_test_case
    def test_mark_no_data(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.int32
        numpy_array = np.full(array_shape, 5, dtype=dtype)
        numpy_array[10, 10] = 999
        numpy_array[20, 20] = 999
        numpy_array[30, 30] = 999

        lue_array = lfr.from_numpy(
            numpy_array, partition_shape=partition_shape, no_data_value=999
        )

        self.assertEqual(lfr.minimum(lue_array).get(), 5)
        self.assertEqual(lfr.maximum(lue_array).get(), 5)

        # TODO Verify that the array contains three no-data elements

    @lue_test.framework_test_case
    def test_small_array(self):

        array_shape = (1, 1)
        partition_shape = (1, 1)
        dtype = np.int32
        numpy_array = np.arange(1, dtype=dtype).reshape(array_shape)

        lue_array = lfr.from_numpy(numpy_array, partition_shape=partition_shape)

        self.assertEqual(lue_array.dtype, dtype)
        self.assertEqual(lue_array.shape, array_shape)
        self.assertEqual(lfr.minimum(lue_array).get(), 0)
        self.assertEqual(lfr.maximum(lue_array).get(), 0)

    @lue_test.framework_test_case
    def test_dtype(self):
        """
        The element type of the numpy array determines the element type of the LUE array. The
        element type of the no-data value must be convertable to the element type of the
        numpy array.
        """
        array_shape = (60, 40)
        partition_shape = (10, 10)

        for input_type in [
            np.uint8,
            np.uint32,
            np.uint64,
        ]:

            input_dtype = np.dtype(input_type)
            numpy_array = np.full(array_shape, 5, dtype=input_dtype)

            lue_array = lfr.from_numpy(numpy_array, partition_shape=partition_shape)
            self.assertEqual(lue_array.dtype, input_dtype)

            lue_array = lfr.from_numpy(
                numpy_array, partition_shape=partition_shape, no_data_value=9
            )
            self.assertEqual(lue_array.dtype, input_dtype)

            self.assertRaises(
                TypeError,
                lfr.from_numpy,
                numpy_array,
                partition_shape=partition_shape,
                no_data_value=-9,
            )
            self.assertRaises(
                TypeError,
                lfr.from_numpy,
                numpy_array,
                partition_shape=partition_shape,
                no_data_value=9.9,
            )

        for input_type in [
            np.int32,
            np.int64,
        ]:

            input_dtype = np.dtype(input_type)
            numpy_array = np.full(array_shape, 5, dtype=input_dtype)

            lue_array = lfr.from_numpy(numpy_array, partition_shape=partition_shape)
            self.assertEqual(lue_array.dtype, input_dtype)

            lue_array = lfr.from_numpy(
                numpy_array, partition_shape=partition_shape, no_data_value=9
            )
            self.assertEqual(lue_array.dtype, input_dtype)

            lue_array = lfr.from_numpy(
                numpy_array, partition_shape=partition_shape, no_data_value=-9
            )
            self.assertEqual(lue_array.dtype, input_dtype)

            self.assertRaises(
                TypeError,
                lfr.from_numpy,
                numpy_array,
                partition_shape=partition_shape,
                no_data_value=9.9,
            )

        for input_type in [
            np.float32,
            np.float64,
        ]:

            input_dtype = np.dtype(input_type)
            numpy_array = np.full(array_shape, 5, dtype=input_dtype)

            lue_array = lfr.from_numpy(numpy_array, partition_shape=partition_shape)
            self.assertEqual(lue_array.dtype, input_dtype)

            lue_array = lfr.from_numpy(
                numpy_array, partition_shape=partition_shape, no_data_value=9.9
            )
            self.assertEqual(lue_array.dtype, input_dtype)
