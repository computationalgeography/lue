import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class FirstNTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_zone = 3
        fill_value = 5.5
        max_nr_cells = 10

        for zone_dtype in [np.uint8, np.uint32, np.uint64, np.int32, np.int64]:
            values = lfr.create_array(array_shape, np.float32, fill_value)
            zones = lfr.create_array(array_shape, zone_dtype, fill_zone)
            route = lfr.decreasing_order(zones, values)

            # TODO
            # RuntimeError: this promise has no valid shared state: HPX(no_state)
            # https://github.com/computationalgeography/lue/issues/629
            # lfr.first_n(route, max_nr_cells)
