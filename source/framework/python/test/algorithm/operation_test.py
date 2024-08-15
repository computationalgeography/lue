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
        cls.array_shape = (60, 40)
        cls.partition_shape = (10, 10)

        cls.numeric_types = [
            np.uint8,
            # np.int8,
            np.uint32,
            np.uint64,
            np.int32,
            np.int64,
            np.float32,
            np.float64,
        ]

        cls.value = {
            np.uint8: np.uint8(8),
            # np.int8: np.int8(-8),
            np.uint32: np.uint32(32),
            np.uint64: np.uint32(64),
            np.int32: np.int64(-32),
            np.int64: np.int64(-64),
            np.float32: np.float32(3.2),
            np.float64: np.float64(6.4),
        }

        cls.array = {
            type_: lfr.create_array(
                cls.array_shape,
                type_,
                fill_value=value,
                partition_shape=cls.partition_shape,
            )
            for type_, value in cls.value.items()
        }

        cls.scalar = {type_: lfr.maximum(array) for type_, array in cls.array.items()}
