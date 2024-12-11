import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class WhereTest(OperationTest):
    @lue_test.framework_test_case
    def test_binary_where_overloads(self):
        # Silly computations. We're only verifying the overloads are
        # available.

        array_shape = (60, 40)
        element_type = lfr.integral_element_types[0]
        fill_value = 5

        condition_array = lfr.create_array(array_shape, lfr.boolean_element_type, 1)

        for element_type in lfr.arithmetic_element_types:
            expression_array = lfr.create_array(array_shape, element_type, fill_value)

            # where(array, true_array)
            result = lfr.where(condition_array, expression_array - 1)
            self.assertTrue(lfr.all(result == 4).future.get())

            # where(array, true_value_f)
            result = lfr.where(condition_array, lfr.maximum(expression_array - 1))
            self.assertTrue(lfr.all(result == 4).future.get())

        # where(array, true_value)
        result = lfr.where(condition_array, 4)
        self.assertTrue(lfr.all(result == 4).future.get())

    @lue_test.framework_test_case
    def test_ternary_where_overloads(self):
        # Silly computations. We're only verifying the overloads are
        # available.

        array_shape = (60, 40)
        fill_value1 = 5
        fill_value2 = 6

        condition_array = lfr.create_array(array_shape, lfr.boolean_element_type, 1)

        for element_type in lfr.arithmetic_element_types:

            expression_array1 = lfr.create_array(array_shape, element_type, fill_value1)
            expression_array2 = lfr.create_array(array_shape, element_type, fill_value2)

            # where(array, true_array, false_array)
            result = lfr.where(
                condition_array, expression_array1 - 1, expression_array2
            )
            self.assertTrue(lfr.all(result == 4).future.get())

            # where(array, true_array, false_value_f)
            result = lfr.where(
                condition_array, expression_array1 - 1, lfr.maximum(expression_array2)
            )
            self.assertTrue(lfr.all(result == 4).future.get())

            # where(array, true_array, false_value)
            result = lfr.where(condition_array, expression_array1 - 1, fill_value2)
            self.assertTrue(lfr.all(result == 4).future.get())

            # where(array, true_value_f, false_array)
            result = lfr.where(
                condition_array, lfr.maximum(expression_array1 - 1), expression_array2
            )
            self.assertTrue(lfr.all(result == 4).future.get())

            # where(array, true_value, false_array)
            result = lfr.where(condition_array, 4, expression_array2)
            self.assertTrue(lfr.all(result == 4).future.get())

            # where(array, true_value_f, false_value_f)
            result = lfr.where(
                condition_array,
                lfr.maximum(expression_array1 - 1),
                lfr.maximum(expression_array2),
            )
            self.assertTrue(lfr.all(result == 4).future.get())

        # where(array, true_value, false_value)
        result = lfr.where(condition_array, 4, 5)
        self.assertTrue(lfr.all(result == 4).future.get())
