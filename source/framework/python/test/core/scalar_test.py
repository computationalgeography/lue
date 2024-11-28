from collections.abc import ValuesView

import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ScalarTest(lue_test.TestCase):
    @classmethod
    @lue_test.framework_test_case
    def setUpClass(cls):
        cls.signed_integral_types = lfr.signed_integral_element_types
        cls.unsigned_integral_types = lfr.unsigned_integral_element_types
        cls.integral_types = lfr.integral_element_types
        cls.floating_point_types = lfr.floating_point_element_types
        cls.numeric_types = lfr.arithmetic_element_types
        cls.value_by_type = lue_test.value_by_type
        cls.dtype_by_floating_point_type = {
            type_: lue_test.dtype_by_type[type_] for type_ in cls.floating_point_types
        }
        cls.dtype_by_type = {
            type_: lue_test.dtype_by_type[type_] for type_ in cls.numeric_types
        }
        cls.scalar_by_dtype = {
            cls.dtype_by_type[type_]: lfr.create_scalar(
                cls.dtype_by_type[type_], cls.value_by_type[type_]
            )
            for type_ in cls.numeric_types
        }

    @lue_test.framework_test_case
    def test_dtype(self):
        for dtype, scalar in self.scalar_by_dtype.items():
            self.assertEqual(scalar.dtype, dtype)

    @lue_test.framework_test_case
    def test_operator_add_overloads(self):
        for type_, dtype in self.dtype_by_type.items():
            value = self.value_by_type[type_]
            scalar = lfr.create_scalar(dtype, value)

            _ = scalar + value
            _ = value + scalar
            scalar += value
            value += scalar

    @lue_test.framework_test_case
    def test_operator_bool(self):
        for _, scalar in self.scalar_by_dtype.items():
            with self.assertRaises(ValueError):
                _ = bool(scalar)

    @lue_test.framework_test_case
    def test_operator_divide_overloads(self):
        for type_, dtype in self.dtype_by_floating_point_type.items():
            value = self.value_by_type[type_]
            scalar = lfr.create_scalar(dtype, value)

            _ = scalar / value
            _ = value / scalar
            scalar /= value
            value /= scalar

    @lue_test.framework_test_case
    def test_operator_multiply_overloads(self):
        for type_, dtype in self.dtype_by_floating_point_type.items():
            value = self.value_by_type[type_]
            scalar = lfr.create_scalar(dtype, value)

            _ = scalar * value
            _ = value * scalar
            scalar *= value
            value *= scalar

    @lue_test.framework_test_case
    def test_operator_pow_overloads(self):
        for type_, dtype in self.dtype_by_floating_point_type.items():
            value = self.value_by_type[type_]
            scalar = lfr.create_scalar(dtype, value)

            _ = scalar**value
            _ = value**scalar
            scalar **= value
            value **= scalar

    @lue_test.framework_test_case
    def test_operator_subtract_overloads(self):
        for type_, dtype in self.dtype_by_type.items():
            value = self.value_by_type[type_]
            scalar = lfr.create_scalar(dtype, value)

            _ = scalar - value
            _ = value - scalar
            scalar -= value
            value -= scalar

    # TODO @lue_test.framework_test_case
    # TODO def test_operator_gt_overloads(self):
    # TODO     for type_ in self.numeric_types:
    # TODO         array = self.array[type_]
    # TODO         scalar = self.scalar[type_]
    # TODO         value = self.value[type_]

    # TODO         _ = array > array
    # TODO         _ = array > scalar
    # TODO         _ = array > value
    # TODO         _ = scalar > array
    # TODO         # _ = value > array

    # TODO @lue_test.framework_test_case
    # TODO def test_operator_ge_overloads(self):
    # TODO     for type_ in self.numeric_types:
    # TODO         array = self.array[type_]
    # TODO         scalar = self.scalar[type_]
    # TODO         value = self.value[type_]

    # TODO         _ = array >= array
    # TODO         _ = array >= scalar
    # TODO         _ = array >= value
    # TODO         _ = scalar >= array
    # TODO         # _ = value >= array

    # TODO @lue_test.framework_test_case
    # TODO def test_operator_eq_overloads(self):
    # TODO     for type_ in self.numeric_types:
    # TODO         array = self.array[type_]
    # TODO         scalar = self.scalar[type_]
    # TODO         value = self.value[type_]

    # TODO         _ = array == array
    # TODO         _ = array == scalar
    # TODO         _ = array == value
    # TODO         _ = scalar == array
    # TODO         # _ = value == array

    # TODO @lue_test.framework_test_case
    # TODO def test_operator_ne_overloads(self):
    # TODO     for type_ in self.numeric_types:
    # TODO         array = self.array[type_]
    # TODO         scalar = self.scalar[type_]
    # TODO         value = self.value[type_]

    # TODO         _ = array != array
    # TODO         _ = array != scalar
    # TODO         _ = array != value
    # TODO         _ = scalar != array
    # TODO         # _ = value != array

    # TODO @lue_test.framework_test_case
    # TODO def test_operator_lt_overloads(self):
    # TODO     for type_ in self.numeric_types:
    # TODO         array = self.array[type_]
    # TODO         scalar = self.scalar[type_]
    # TODO         value = self.value[type_]

    # TODO         _ = array < array
    # TODO         _ = array < scalar
    # TODO         _ = array < value
    # TODO         _ = scalar < array
    # TODO         # _ = value < array

    # TODO @lue_test.framework_test_case
    # TODO def test_operator_le_overloads(self):
    # TODO     for type_ in self.numeric_types:
    # TODO         array = self.array[type_]
    # TODO         scalar = self.scalar[type_]
    # TODO         value = self.value[type_]

    # TODO         _ = array <= array
    # TODO         _ = array <= scalar
    # TODO         _ = array <= value
    # TODO         _ = scalar <= array
    # TODO         # _ = value <= array
