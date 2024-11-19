import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class CreatePartitionedArrayTest(OperationTest):
    @lue_test.framework_test_case
    def test_operator_add_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array + array
            _ = array + scalar
            _ = array + value
            _ = scalar + array
            _ = value + array

    @lue_test.framework_test_case
    def test_operator_sub_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array - array
            _ = array - scalar
            _ = array - value
            _ = scalar - array
            _ = value - array

    @lue_test.framework_test_case
    def test_operator_mul_overloads(self):
        for element_type in lfr.floating_point_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array * array
            _ = array * scalar
            _ = array * value
            _ = scalar * array
            _ = value * array

    @lue_test.framework_test_case
    def test_operator_div_overloads(self):
        for element_type in lfr.floating_point_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array / array
            _ = array / scalar
            _ = array / value
            _ = scalar / array
            _ = value / array

    @lue_test.framework_test_case
    def test_operator_gt_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array > array
            _ = array > scalar
            _ = array > value
            _ = scalar > array
            # _ = value > array

    @lue_test.framework_test_case
    def test_operator_ge_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array >= array
            _ = array >= scalar
            _ = array >= value
            _ = scalar >= array
            # _ = value >= array

    @lue_test.framework_test_case
    def test_operator_eq_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array == array
            _ = array == scalar
            _ = array == value
            _ = scalar == array
            # _ = value == array

    @lue_test.framework_test_case
    def test_operator_ne_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array != array
            _ = array != scalar
            _ = array != value
            _ = scalar != array
            # _ = value != array

    @lue_test.framework_test_case
    def test_operator_lt_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array < array
            _ = array < scalar
            _ = array < value
            _ = scalar < array
            # _ = value < array

    @lue_test.framework_test_case
    def test_operator_le_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array <= array
            _ = array <= scalar
            _ = array <= value
            _ = scalar <= array
            # _ = value <= array

    @lue_test.framework_test_case
    def test_operator_abs_overloads(self):
        for element_type in lfr.signed_integral_element_types:
            array = self.array[element_type]

            _ = abs(array)

    @lue_test.framework_test_case
    def test_operator_pow_overloads(self):
        for element_type in lfr.floating_point_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array**array
            _ = array**scalar
            _ = array**value
            _ = scalar**array
            _ = value**array

    @lue_test.framework_test_case
    def test_operator_bool_overloads(self):
        for element_type in lfr.arithmetic_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]

            with self.assertRaises(ValueError):
                _ = bool(array)
                _ = bool(scalar)

    @lue_test.framework_test_case
    def test_operator_negate_overloads(self):
        for element_type in lfr.signed_integral_element_types:
            array = self.array[element_type]
            _ = -array

    @lue_test.framework_test_case
    def test_operator_logical_not_overloads(self):
        for element_type in lfr.integral_element_types:
            array = self.array[element_type]
            _ = ~array

    @lue_test.framework_test_case
    def test_operator_mod_overloads(self):
        for element_type in lfr.integral_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = array % array
            _ = array % scalar
            _ = array % value
            _ = scalar % array
            _ = value % array
