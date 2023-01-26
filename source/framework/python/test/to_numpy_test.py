import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ToNumPyTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_array(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.int32
        fill_value = 5
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)
        numpy_array = lfr.to_numpy(array)

        self.assertEqual(numpy_array.dtype, dtype)
        np.testing.assert_array_equal(
            numpy_array, np.full(array_shape, fill_value, dtype=dtype)
        )

    @lue_test.framework_test_case
    def test_mark_no_data(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.int32

        # Create array containing only no-data elements
        array = lfr.where(
            lfr.create_array(array_shape, partition_shape, dtype, 5) != 5, 7
        )

        numpy_array = lfr.to_numpy(array, 9)

        np.testing.assert_array_equal(numpy_array, np.full(array_shape, 9, dtype=dtype))

    @lue_test.framework_test_case
    def test_small_array(self):

        array_shape = (1, 1)
        partition_shape = (1, 1)
        dtype = np.int32
        fill_value = 5
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)
        numpy_array = lfr.to_numpy(array)

        self.assertEqual(numpy_array.dtype, dtype)
        np.testing.assert_array_equal(
            numpy_array, np.full(array_shape, fill_value, dtype=dtype)
        )

    @lue_test.framework_test_case
    def test_dtype(self):
        """
        The element type of the LUE array determines the element type of the numpy array. The
        element type of the no-data value must be convertable to the element type of the LUE array.
        """
        array_shape = (60, 40)
        partition_shape = (10, 10)

        for input_type in [
            np.uint8,
            np.uint32,
            np.uint64,
        ]:

            input_dtype = np.dtype(input_type)
            lue_array = lfr.create_array(
                array_shape, partition_shape, input_dtype, input_type(5)
            )

            numpy_array = lfr.to_numpy(lue_array)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, 9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            self.assertRaises(TypeError, lfr.to_numpy, lue_array, -9)
            self.assertRaises(TypeError, lfr.to_numpy, lue_array, 9.9)

        for input_type in [
            np.int32,
            np.int64,
        ]:

            input_dtype = np.dtype(input_type)
            lue_array = lfr.create_array(
                array_shape, partition_shape, input_dtype, input_type(5)
            )

            numpy_array = lfr.to_numpy(lue_array)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, 9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, -9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            self.assertRaises(TypeError, lfr.to_numpy, lue_array, 9.9)

        for input_type in [
            np.float32,
            np.float64,
        ]:

            input_dtype = np.dtype(input_type)
            lue_array = lfr.create_array(
                array_shape, partition_shape, input_dtype, input_type(5)
            )

            numpy_array = lfr.to_numpy(lue_array)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, 9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, -9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, 9.9)
            self.assertEqual(numpy_array.dtype, input_dtype)

    @lue_test.framework_test_case
    def test_result_of_multiple_operations(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.dtype(np.int32)
        fill_value = 5
        array = lfr.create_array(array_shape, partition_shape, dtype, fill_value) + 5
        numpy_array = lfr.to_numpy(array)

        self.assertEqual(numpy_array.dtype, dtype)
        np.testing.assert_array_equal(
            numpy_array, np.full(array_shape, fill_value + 5, dtype=dtype)
        )

    @lue_test.framework_test_case
    def test_numpy_roundtrip(self):

        array_shape = (60, 40)
        nr_cells = 60 * 40
        partition_shape = (10, 10)

        for type_ in [np.int32, np.float32]:
            dtype = np.dtype(type_)
            numpy_array = np.arange(nr_cells, dtype=dtype).reshape(array_shape)

            lue_array = lfr.from_numpy(numpy_array, partition_shape)
            numpy_array = lfr.to_numpy(lue_array)

            self.assertEqual(numpy_array.dtype, dtype)
            np.testing.assert_array_equal(
                numpy_array,
                np.arange(nr_cells, dtype=dtype).reshape(array_shape),
                err_msg="Error in case type is {}".format(dtype),
                verbose=True,
            )

    @lue_test.framework_test_case
    def test_numpy_roundtrip_result_of_multiple_operations(self):

        array_shape = (60, 40)
        nr_cells = 60 * 40
        partition_shape = (10, 10)

        for type_ in [np.int32, np.float32]:
            dtype = np.dtype(type_)
            numpy_array = np.arange(nr_cells, dtype=dtype).reshape(array_shape)

            lue_array = lfr.from_numpy(numpy_array, partition_shape) + 5
            numpy_array = lfr.to_numpy(lue_array)

            self.assertEqual(numpy_array.dtype, dtype)
            np.testing.assert_array_equal(
                numpy_array,
                np.arange(nr_cells, dtype=dtype).reshape(array_shape) + 5,
                err_msg="Error in case type is {}".format(dtype),
                verbose=True,
            )
