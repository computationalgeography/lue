import lue.framework as lfr
import lue_test
import numpy as np
import itertools


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ZonalMajorityTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        fill_value = 5
        fill_zone = 3

        for value_dtype, zone_dtype in itertools.product(
            (np.uint8, np.int32, np.uint32, np.int64, np.uint64),
            (np.uint8, np.int32, np.uint32, np.int64, np.uint64),
        ):
            array = lfr.create_array(
                array_shape, partition_shape, value_dtype, fill_value
            )
            zones = lfr.create_array(
                array_shape, partition_shape, zone_dtype, fill_zone
            )
            lfr.zonal_majority(array, zones)
