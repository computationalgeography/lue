import numpy as np

import lue.framework as lfr
import lue.image_land as img
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class IntegrateTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        zone_fill_value = 1
        integrand_fill_value = 5.5
        max_nr_cells = 10

        for zone_dtype in [np.uint8, np.uint32, np.uint64, np.int32, np.int64]:
            for integrand_dtype in [np.float32, np.float64]:
                zone = lfr.create_array(array_shape, zone_dtype, zone_fill_value)
                field = lfr.create_array(
                    array_shape, integrand_dtype, integrand_fill_value
                )
                integrand = lfr.create_array(
                    array_shape, integrand_dtype, integrand_fill_value
                )
                route = lfr.decreasing_order(zone, field, max_nr_cells)

                img.integrate(route, integrand, max_nr_cells)
