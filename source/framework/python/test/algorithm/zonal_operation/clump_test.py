import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ClumpTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_zone = 3

        for zone_dtype in [np.uint8, np.uint32, np.uint64, np.int32, np.int64]:
            zones = lfr.create_array(array_shape, zone_dtype, fill_zone)

            for connectivity in [
                lfr.Connectivity.diagonal,
                lfr.Connectivity.nondiagonal,
            ]:
                lfr.clump(zones, connectivity)
