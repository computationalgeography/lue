import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class UniformTest(lue_test.TestCase):

    @lue_test.framework_test_case
    def test_overload1(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        min_value = 33
        max_value = 55
        dtype = np.dtype(np.float32)
        fill_value = 5.5

        # array = lfr.create_array(array_shape, partition_shape, dtype, fill_value)

        # for type_ in [np.uint32, np.uint64, np.int32, np.int64, np.float32, np.float64]:
        # TODO Support dtype argument
        #     lfr.uniform(array, min_value, np.dtype(type_), max_value)


    @lue_test.framework_test_case
    def test_overload2(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        min_value = 33
        max_value = 55

        for type_ in [np.uint32, np.uint64, np.int32, np.int64, np.float32, np.float64]:
            lfr.uniform(array_shape, partition_shape, np.dtype(type_), min_value, max_value)
