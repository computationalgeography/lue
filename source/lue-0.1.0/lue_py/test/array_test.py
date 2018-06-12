from functools import reduce
import os
import numpy
import lue
import lue_test


omnipresent = lue.constant_collection.time.omnipresent


class ArrayTest(lue_test.TestCase):

    def setUp(self):

        dataset_name = "my_dataset.lue"
        lue_test.remove_file_if_existant(dataset_name)
        self.dataset = lue.create_dataset(dataset_name)
        phenomenon = self.dataset.add_phenomenon("my_phenomenon")
        self.property_set = lue.constant_collection.create_property_set(phenomenon,
            "my_property_set")
        self.nr_items = 5
        self.nr_rows = 3
        self.nr_cols = 2
        self.value_shape = (self.nr_rows, self.nr_cols)
        self.value_type = numpy.int32

        self.property_set.ids.reserve(self.nr_items)[:] = \
            numpy.arange(self.nr_items)

        property = omnipresent.same_shape.create_property(
            self.property_set, "my_property", self.value_type, self.value_shape)

        self.lue_values = property.values.reserve(self.nr_items)
        self.numpy_values = numpy.arange(
            self.nr_items * reduce(
                lambda x, y: x * y, self.value_shape),
            dtype=self.value_type).reshape(
                (self.nr_items,) + self.value_shape)
        self.lue_values[:] = self.numpy_values

        lue.assert_is_valid(self.dataset)


    def tearDown(self):

        del self.dataset


    def test_no_index(self):

        # Shape of values is: (nr_items, nr_rows, nr_cols)
        def compare_values():

            # Verify current value
            self.assertArraysEqual(self.lue_values[:], self.numpy_values)
            self.assertArraysEqual(self.lue_values[:], self.numpy_values[:])


            # With step
            self.assertArraysEqual(
                self.lue_values[::2],
                self.numpy_values[::2])


        compare_values()


        # Update value, by array
        self.numpy_values[:] = self.numpy_values + 5000
        self.lue_values[:] = self.lue_values[:] + 5000
        compare_values()


        # With step
        self.numpy_values[::2] = self.numpy_values[::2] + 5500
        self.lue_values[::2] = self.lue_values[::2] + 5500
        compare_values()


        # Update value, by value
        self.numpy_values[:] = 6000
        self.lue_values[:] = 6000
        compare_values()


        # # With step
        # self.numpy_values[::2] = 6500
        # self.lue_values[::2] = 6500
        # compare_values()


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


        # Update value
        new_numpy_values = self.numpy_values + 5000


        # Value of first item
        self.lue_values[0:1] = new_numpy_values[0:1]

        self.assertArraysEqual(
            self.lue_values[0:1],
            new_numpy_values[0:1])


        # Value of last item
        self.lue_values[-1:self.nr_items] = new_numpy_values[-1:self.nr_items]

        self.assertArraysEqual(
            self.lue_values[-1:self.nr_items],
            new_numpy_values[-1:self.nr_items])


        # Values of all items
        self.lue_values[0:self.nr_items] = new_numpy_values[0:self.nr_items]

        self.assertArraysEqual(
            self.lue_values[0:self.nr_items],
            new_numpy_values[0:self.nr_items])


        # Too negative index
        # self.assertRaises(
        self.lue_values[-self.nr_items]  # OK
        # self.lue_values[-self.nr_items-1]  # NOT_OK
        self.assertRaises(
            IndexError, self.lue_values.__getitem__, -self.nr_items-1)


    def test_one_integer_index(self):

        # Value of an item
        for i in range(self.nr_items):
            self.assertArraysEqual(
                self.lue_values[i],
                self.numpy_values[i])

            self.assertArraysEqual(
                self.lue_values[0 - i],
                self.numpy_values[0 - i])


        # Update value
        new_numpy_values = self.numpy_values + 5000

        # Value of an item
        for i in range(self.nr_items):
            self.lue_values[i] = new_numpy_values[i]

            self.assertArraysEqual(
                self.lue_values[i],
                new_numpy_values[i])


            self.lue_values[-i] = new_numpy_values[-i]

            self.assertArraysEqual(
                self.lue_values[0 - i],
                new_numpy_values[0 - i])


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


        # Update value
        new_numpy_values = self.numpy_values + 5000

        # TODO Test assign


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

        # Update value
        # TODO Test assign


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

        # Update value
        # TODO Test assign


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

        # Update value
        # TODO Test assign


    def test_too_many_indices(self):

        with self.assertRaises(IndexError) as numpy_context:
            self.numpy_values[0, 0, 0, 0]

        with self.assertRaises(IndexError) as lue_context:
            self.lue_values[0, 0, 0, 0]

        # Verify error message is similar to what numpy reports
        self.assertTrue(
            str(numpy_context.exception).startswith("too many indices"))
        self.assertTrue(
            str(lue_context.exception).startswith("too many indices"))

        # Update value
        # TODO Test assign


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

        # Update value
        # TODO Test assign


    def test_iterate(self):

        with self.assertRaises(ValueError) as lue_context:
            for _ in self.property_set.ids:
                pass

        self.assertEqual(
            str(lue_context.exception),
            "on the fly iteration not supported, use the numpy array")

        with self.assertRaises(ValueError) as lue_context:
            for _ in self.lue_values:
                pass

        self.assertEqual(
            str(lue_context.exception),
            "on the fly iteration not supported, use the numpy array")

