import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ZonalAreaTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        fill_zone = 3

        for zone_dtype in [np.uint8, np.uint32, np.uint64, np.int32, np.int64]:
            zones = lfr.create_array(
                array_shape, partition_shape, zone_dtype, fill_zone
            )
            lfr.zonal_area(zones)
