import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class CastTest(lue_test.TestCase):

    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        fill_value = 5

        for input_type in [np.uint64, np.int64]:
            input_array = lfr.create_array(array_shape, partition_shape, np.dtype(input_type), fill_value)

            for output_type in [np.float32, np.float64]:
                output_array = lfr.cast(input_array, np.dtype(output_type))
