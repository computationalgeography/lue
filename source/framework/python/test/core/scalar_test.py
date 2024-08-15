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
        cls.signed_integral_types = [
            # np.int8,
            np.int32,
            np.int64,
        ]
        cls.unsigned_integral_types = [
            np.uint8,
            np.uint32,
            np.uint64,
        ]
        cls.integral_types = cls.signed_integral_types + cls.unsigned_integral_types
        cls.floating_point_types = [
            np.float32,
            np.float64,
        ]

        cls.numeric_types = cls.integral_types + cls.floating_point_types

        cls.value_by_type = {
            np.uint8: np.uint8(8),
            # np.int8: np.int8(-8),
            np.uint32: np.uint32(32),
            np.uint64: np.uint32(64),
            np.int32: np.int64(-32),
            np.int64: np.int64(-64),
            np.float32: np.float32(3.2),
            np.float64: np.float64(6.4),
        }

        cls.dtype_by_floating_point_type = {
            type_: np.dtype(type_) for type_ in cls.floating_point_types
        }

        cls.dtype_by_type = {type_: np.dtype(type_) for type_ in cls.numeric_types}

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
