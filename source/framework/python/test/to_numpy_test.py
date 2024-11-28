import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ToNumPyTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_array(self):
        array_shape = (60, 40)
        element_type = lfr.signed_integral_element_types[0]
        fill_value = 5
        array = lfr.create_array(array_shape, element_type, fill_value)
        numpy_array = lfr.to_numpy(array)

        self.assertEqual(numpy_array.dtype, element_type)
        np.testing.assert_array_equal(
            numpy_array, np.full(array_shape, fill_value, dtype=element_type)
        )

    @lue_test.framework_test_case
    def test_mark_no_data(self):
        array_shape = (60, 40)
        element_type = lfr.signed_integral_element_types[0]

        # Create array containing only no-data elements
        array = lfr.where(lfr.create_array(array_shape, element_type, 5) != 5, 7)

        numpy_array = lfr.to_numpy(array, 9)

        np.testing.assert_array_equal(
            numpy_array, np.full(array_shape, 9, dtype=element_type)
        )

    @lue_test.framework_test_case
    def test_small_array(self):
        array_shape = (1, 1)
        element_type = lfr.signed_integral_element_types[0]
        fill_value = 5
        array = lfr.create_array(array_shape, element_type, fill_value)
        numpy_array = lfr.to_numpy(array)

        self.assertEqual(numpy_array.dtype, element_type)
        np.testing.assert_array_equal(
            numpy_array, np.full(array_shape, fill_value, dtype=element_type)
        )

    @lue_test.framework_test_case
    def test_dtype(self):
        """
        The element type of the LUE array determines the element type of the numpy array. The
        element type of the no-data value must be convertible to the element type of the LUE array.
        """
        array_shape = (60, 40)

        for input_type in lfr.unsigned_integral_element_types:
            input_dtype = np.dtype(input_type)
            lue_array = lfr.create_array(array_shape, input_dtype, input_type(5))

            numpy_array = lfr.to_numpy(lue_array)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, 9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            self.assertRaises(TypeError, lfr.to_numpy, lue_array, -9)
            self.assertRaises(TypeError, lfr.to_numpy, lue_array, 9.9)

        for input_type in lfr.signed_integral_element_types:
            input_dtype = np.dtype(input_type)
            lue_array = lfr.create_array(array_shape, input_dtype, input_type(5))

            numpy_array = lfr.to_numpy(lue_array)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, 9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            numpy_array = lfr.to_numpy(lue_array, -9)
            self.assertEqual(numpy_array.dtype, input_dtype)

            self.assertRaises(TypeError, lfr.to_numpy, lue_array, 9.9)

        for input_type in lfr.floating_point_element_types:
            input_dtype = np.dtype(input_type)
            lue_array = lfr.create_array(array_shape, input_dtype, input_type(5))

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
        dtype = np.dtype(lfr.signed_integral_element_types[0])
        fill_value = 5
        array = lfr.create_array(array_shape, dtype, fill_value) + 5
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

        for element_type in lfr.arithmetic_element_types:
            if element_type not in [np.int8, np.uint8]:
                dtype = np.dtype(element_type)
                numpy_array = np.arange(nr_cells, dtype=dtype).reshape(array_shape)

                lue_array = lfr.from_numpy(numpy_array, partition_shape=partition_shape)
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

        for element_type in lfr.arithmetic_element_types:
            if element_type not in [np.int8, np.uint8]:
                dtype = np.dtype(element_type)
                numpy_array = np.arange(nr_cells, dtype=dtype).reshape(array_shape)

                lue_array = (
                    lfr.from_numpy(numpy_array, partition_shape=partition_shape) + 5
                )
                numpy_array = lfr.to_numpy(lue_array)

                self.assertEqual(numpy_array.dtype, dtype)
                np.testing.assert_array_equal(
                    numpy_array,
                    np.arange(nr_cells, dtype=dtype).reshape(array_shape) + 5,
                    err_msg="Error in case type is {}".format(dtype),
                    verbose=True,
                )
