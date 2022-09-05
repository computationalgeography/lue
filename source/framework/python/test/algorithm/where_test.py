import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class WhereTest(lue_test.TestCase):

    @lue_test.framework_test_case
    def test_binary_where_overloads(self):
        # Silly computations. We're only verifying the overloads are
        # available.

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.dtype(np.int32)
        fill_value = 5
        expression_array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)

        condition_array = expression_array == 5

        # where(array, true_array)
        result = lfr.where(condition_array, expression_array - 1)
        self.assertTrue(lfr.all(result == 4).get())

        # where(array, true_value_f)
        result = lfr.where(condition_array, lfr.maximum(expression_array - 1))
        self.assertTrue(lfr.all(result == 4).get())

        # where(array, true_value)
        result = lfr.where(condition_array, 4)
        self.assertTrue(lfr.all(result == 4).get())


    @lue_test.framework_test_case
    def test_ternary_where_overloads(self):
        # Silly computations. We're only verifying the overloads are
        # available.

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.dtype(np.int32)
        fill_value1 = 5
        fill_value2 = 6
        expression_array1 = lfr.create_array(array_shape, partition_shape, dtype, fill_value1)
        expression_array2 = lfr.create_array(array_shape, partition_shape, dtype, fill_value2)

        condition_array = expression_array1 == 5

        # where(array, true_array, false_array)
        result = lfr.where(condition_array, expression_array1 - 1, expression_array2)
        self.assertTrue(lfr.all(result == 4).get())


        # where(array, true_array, false_value_f)
        result = lfr.where(condition_array, expression_array1 - 1, lfr.maximum(expression_array2))
        self.assertTrue(lfr.all(result == 4).get())

        # where(array, true_array, false_value)
        result = lfr.where(condition_array, expression_array1 - 1, fill_value2)
        self.assertTrue(lfr.all(result == 4).get())


        # where(array, true_value_f, false_array)
        result = lfr.where(condition_array, lfr.maximum(expression_array1 - 1), expression_array2)
        self.assertTrue(lfr.all(result == 4).get())

        # where(array, true_value, false_array)
        result = lfr.where(condition_array, 4, expression_array2)
        self.assertTrue(lfr.all(result == 4).get())


        # where(array, true_value_f, false_value_f)
        result = lfr.where(condition_array, lfr.maximum(expression_array1 - 1), lfr.maximum(expression_array2))
        self.assertTrue(lfr.all(result == 4).get())

        # where(array, true_value, false_value)
        result = lfr.where(condition_array, 4, 5)
        self.assertTrue(lfr.all(result == 4).get())
