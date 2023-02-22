import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class CreatePartitionedArrayTest(lue_test.TestCase):
    @classmethod
    @lue_test.framework_test_case
    def setUpClass(cls):
        cls.value = 5.5
        cls.array = lfr.create_array(
            array_shape=(60, 40),
            partition_shape=(10, 10),
            dtype=np.float32,
            fill_value=cls.value,
        )
        cls.value_f = lfr.maximum(cls.array)

    @lue_test.framework_test_case
    def test_operator_add_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array + array
        tmp = array + value
        tmp = array + value_f
        tmp = value + array
        tmp = value_f + array

    @lue_test.framework_test_case
    def test_operator_sub_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array - array
        tmp = array - value
        tmp = array - value_f
        tmp = value - array
        tmp = value_f - array

    @lue_test.framework_test_case
    def test_operator_mul_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array * array
        tmp = array * value
        tmp = array * value_f
        tmp = value * array
        tmp = value_f * array

    @lue_test.framework_test_case
    def test_operator_div_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array / array
        tmp = array / value
        tmp = array / value_f
        tmp = value / array
        tmp = value_f / array

    @lue_test.framework_test_case
    def test_operator_gt_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array > array
        tmp = array > value
        tmp = array > value_f
        tmp = value > array
        tmp = value_f > array

    @lue_test.framework_test_case
    def test_operator_ge_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array >= array
        tmp = array >= value
        tmp = array >= value_f
        tmp = value >= array
        tmp = value_f >= array

    @lue_test.framework_test_case
    def test_operator_eq_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array == array
        tmp = array == value
        tmp = array == value_f
        tmp = value == array
        tmp = value_f == array

    @lue_test.framework_test_case
    def test_operator_ne_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array != array
        tmp = array != value
        tmp = array != value_f
        tmp = value != array
        tmp = value_f != array

    @lue_test.framework_test_case
    def test_operator_lt_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array < array
        tmp = array < value
        tmp = array < value_f
        tmp = value < array
        tmp = value_f < array

    @lue_test.framework_test_case
    def test_operator_le_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array <= array
        tmp = array <= value
        tmp = array <= value_f
        tmp = value <= array
        tmp = value_f <= array

    @lue_test.framework_test_case
    def test_operator_abs_overloads(self):
        array = self.array
        tmp = abs(array)

    @lue_test.framework_test_case
    def test_operator_pow_overloads(self):
        array, value, value_f = self.array, self.value, self.value_f
        tmp = array**array
        tmp = array**value
        tmp = array**value_f
        tmp = value**array
        tmp = value_f**array

    @lue_test.framework_test_case
    def test_operator_bool(self):
        array = self.array
        with self.assertRaises(ValueError):
            tmp = bool(array)
