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
        cls.signed_integral_types = lue_test.signed_integral_types
        cls.unsigned_integral_types = lue_test.unsigned_integral_types
        cls.integral_types = lue_test.integral_types
        cls.floating_point_types = lue_test.floating_point_types
        cls.numeric_types = lue_test.numeric_types
        cls.value_by_type = lue_test.value_by_type
        cls.dtype_by_floating_point_type = lue_test.dtype_by_floating_point_type
        cls.dtype_by_type = lue_test.dtype_by_type

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
