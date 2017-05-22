from functools import reduce
import os
import numpy
import lue
import lue_test


omnipresent = lue.constant_size.time.omnipresent


class ArrayTest(lue_test.TestCase):

    def setUp(self):

        dataset_name = "my_dataset.lue"
        lue_test.remove_file_if_existant(dataset_name)
        self.dataset = lue.create_dataset(dataset_name)
        phenomenon = self.dataset.add_phenomenon("my_phenomenon")
        property_set = omnipresent.create_property_set(phenomenon,
            "my_property_set")
        self.nr_items = 5
        self.nr_rows = 3
        self.nr_cols = 2
        value_shape = (self.nr_rows, self.nr_cols)
        value_type = numpy.int32
        property = omnipresent.same_shape.create_property(
            property_set, "my_property", value_type, value_shape)

        self.lue_values = property.reserve(self.nr_items)
        self.numpy_values = numpy.arange(
            self.nr_items * reduce(
                lambda x, y: x * y, value_shape),
            dtype=value_type).reshape((self.nr_items,) + value_shape)
        self.lue_values[:] = self.numpy_values


    def tearDown(self):

        del self.dataset


    def test_no_index(self):

        # Shape of values is: (nr_items, nr_rows, nr_cols)
        self.assertArraysEqual(self.lue_values[:], self.numpy_values)
        self.assertArraysEqual(self.lue_values[:], self.numpy_values[:])

        self.assertArraysEqual(
            self.lue_values[::2],
            self.numpy_values[::2])


    def test_one_slice_index(self):

        # Value of first item
        # [[0 1]
        #  [2 3]
        #  [4 5]]
        self.assertArraysEqual(
            self.lue_values[0:1],
            self.numpy_values[0:1])

        # Values of all items
        self.assertArraysEqual(
            self.lue_values[0:self.nr_items],
            self.numpy_values[0:self.nr_items])

        # Value of last item
        # Negative index
        self.assertArraysEqual(
            self.lue_values[-1:self.nr_items],
            self.numpy_values[-1:self.nr_items])


    def test_one_integer_index(self):

        # Value of an item
        for i in range(self.nr_items):
            self.assertArraysEqual(
                self.lue_values[i],
                self.numpy_values[i])

            self.assertArraysEqual(
                self.lue_values[0 - i],
                self.numpy_values[0 - i])


    def test_two_slice_indices(self):

        nr_items = self.nr_items
        nr_rows = self.nr_rows

        # First row of first item
        self.assertArraysEqual(
            self.lue_values[0:1, 0:1],
            self.numpy_values[0:1, 0:1])

        # Last row of last item
        self.assertArraysEqual(
            self.lue_values[nr_items-1:nr_items, nr_rows-1:nr_rows],
            self.numpy_values[nr_items-1:nr_items, nr_rows-1:nr_rows])

        # Last row of last item
        self.assertArraysEqual(
            self.lue_values[-1:, -1:],
            self.numpy_values[-1:, -1:])

    def test_two_integer_indices(self):

        nr_items = self.nr_items
        nr_rows = self.nr_rows

        # First row of first item
        self.assertArraysEqual(
            self.lue_values[0, 0],
            self.numpy_values[0, 0])

        # Last row of last item
        self.assertArraysEqual(
            self.lue_values[nr_items-1, nr_rows-1],
            self.numpy_values[nr_items-1, nr_rows-1])

        # Last row of last item
        self.assertArraysEqual(
            self.lue_values[-1, -1],
            self.numpy_values[-1, -1])


    def test_three_slice_indices(self):

        nr_items = self.nr_items
        nr_rows = self.nr_rows
        nr_cols = self.nr_cols

        # First cell of first item
        self.assertArraysEqual(
            self.lue_values[0:1, 0:1, 0:1],
            self.numpy_values[0:1, 0:1, 0:1])

        # Last cell of last item
        self.assertArraysEqual(
            self.lue_values[nr_items-1:nr_items, nr_rows-1:nr_rows,
                nr_cols-1:nr_cols],
            self.numpy_values[nr_items-1:nr_items, nr_rows-1:nr_rows,
                nr_cols-1:nr_cols])

        # Last cell of last item
        self.assertArraysEqual(
            self.lue_values[-1:, -1:, -1:],
            self.numpy_values[-1:, -1:, -1:])


    def test_three_integer_indices(self):

        nr_items = self.nr_items
        nr_rows = self.nr_rows
        nr_cols = self.nr_cols

        # First cell of first item
        self.assertArraysEqual(
            self.lue_values[0, 0, 0],
            self.numpy_values[0, 0, 0])

        # Last cell of last item
        self.assertArraysEqual(
            self.lue_values[nr_items-1, nr_rows-1, nr_cols-1],
            self.numpy_values[nr_items-1, nr_rows-1, nr_cols-1])

        # Last cell of last item
        self.assertArraysEqual(
            self.lue_values[-1, -1, -1],
            self.numpy_values[-1, -1, -1])


    def test_too_many_indices(self):

        with self.assertRaises(IndexError) as numpy_context:
            self.numpy_values[0, 0, 0, 0]

        with self.assertRaises(IndexError) as lue_context:
            self.lue_values[0, 0, 0, 0]

        self.assertEqual(
            str(lue_context.exception),
            str(numpy_context.exception))


    def test_unsupported_index_type(self):

        # String
        with self.assertRaises(IndexError) as lue_context:
            self.lue_values["meh"]

        self.assertEqual(
            str(lue_context.exception),
            "only integers and slices (`:`) are valid indices")

        # Float
        with self.assertRaises(IndexError) as lue_context:
            self.lue_values[5.5]

        self.assertEqual(
            str(lue_context.exception),
            "only integers and slices (`:`) are valid indices")
