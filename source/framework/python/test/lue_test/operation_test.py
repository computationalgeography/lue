import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class OperationTest(lue_test.TestCase):
    @classmethod
    @lue_test.framework_test_case
    def setUpClass(cls):
        cls.boolean_types = [lfr.boolean_element_type]  # lue_test.boolean_types
        cls.unsigned_integral_types = (
            lfr.unsigned_integral_element_types
        )  # lue_test.unsigned_integral_types
        cls.signed_integral_types = (
            lfr.signed_integral_element_types
        )  # lue_test.signed_integral_types
        cls.integral_types = lfr.integral_element_types  # lue_test.integral_types
        cls.floating_point_types = (
            lfr.floating_point_element_types
        )  # lue_test.floating_point_types
        cls.numeric_types = lfr.arithmetic_element_types  # lue_test.numeric_types

        cls.value = {
            type_: lue_test.value_by_type[type_] for type_ in cls.numeric_types
        }

        cls.array_shape = (60, 40)
        cls.partition_shape = (10, 10)

        cls.array = {
            type_: lfr.create_array(
                cls.array_shape,
                type_,
                fill_value=value,
                partition_shape=cls.partition_shape,
            )
            for type_, value in cls.value.items()
        }

        # cls.scalar = {type_: lfr.maximum(array) for type_, array in cls.array.items()}

        cls.dtype_by_type = {
            type_: lue_test.dtype_by_type[type_] for type_ in cls.numeric_types
        }

        cls.scalar = {
            type_: lfr.create_scalar(cls.dtype_by_type[type_], cls.value[type_])
            for type_ in cls.numeric_types
        }

    def assert_overload(self, function, *args, **kwargs):
        results = function(*args, **kwargs)

        if isinstance(results, tuple):
            self.assertTrue(all(result is not None for result in results), results)
        else:
            self.assertIsNotNone(results)
