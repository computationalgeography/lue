import numpy as np

import lue.framework as lfr
import lue.image_land as img
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class IntegrateAndAllocateTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        zone_fill_value = 1
        suitability_fill_value = 5.5
        max_nr_cells = 10
        nr_crops = 3

        for zone_dtype in [np.uint8, np.uint32, np.uint64, np.int32, np.int64]:
            for floating_point_type in [np.float32, np.float64]:
                zone = lfr.create_array(array_shape, zone_dtype, zone_fill_value)
                suitability = lfr.create_array(
                    array_shape, floating_point_type, suitability_fill_value
                )
                route = lfr.decreasing_order(zone, suitability, max_nr_cells)

                crop_fractions = [
                    lfr.create_array(array_shape, floating_point_type, zone_fill_value),
                    lfr.create_array(array_shape, floating_point_type, zone_fill_value),
                    lfr.create_array(array_shape, floating_point_type, zone_fill_value),
                ]

                (
                    crop_fractions_we_got,
                    zonal_production_we_got,
                ) = img.integrate_and_allocate(route, zone, crop_fractions)

                self.assertTrue(isinstance(crop_fractions_we_got, list))
                # TODO self.assertEqual(len(crop_fractions_we_got), nr_crops)

                self.assertTrue(isinstance(zonal_production_we_got, dict))
                # TODO self.assertEqual(len(zonal_production_we_got), nr_crops)
