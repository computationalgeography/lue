import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ZonalNormalTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        fill_zone = 3

        for value_dtype, zone_dtype in [
            (np.float32, np.uint8),
            (np.float32, np.uint32),
            (np.float32, np.uint64),
            (np.float64, np.uint8),
            (np.float64, np.uint32),
            (np.float64, np.uint64),
        ]:
            zones = lfr.create_array(
                array_shape, partition_shape, zone_dtype, fill_zone
            )
            lfr.zonal_normal(zones, value_dtype)
