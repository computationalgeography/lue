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
        zone_fill_value = 3
        suitability_fill_value = 5.5
        max_nr_cells = 10
        nr_crops = 3

        sdp_factor_fill_value = 3.3
        yield_factor_fill_value = 4.4
        crop_fraction_fill_value = 0.5
        production_fill_value = 5.5
        demand_fill_value = 5.5
        nr_zones = 1

        for zone_element_type in lfr.zone_element_types:
            print(zone_element_type)
            zones = [zone_element_type(zone_fill_value)]

            for floating_point_element_type in lfr.floating_point_element_types:
                print(floating_point_element_type)
                zone = lfr.create_array(array_shape, zone_element_type, zone_fill_value)
                suitability = lfr.create_array(
                    array_shape, floating_point_element_type, suitability_fill_value
                )
                route = lfr.decreasing_order(
                    zone, suitability, max_nr_cells=max_nr_cells
                )

                sdp_factors_per_crop = [
                    lfr.create_array(
                        array_shape, floating_point_element_type, sdp_factor_fill_value
                    ),
                    lfr.create_array(
                        array_shape, floating_point_element_type, sdp_factor_fill_value
                    ),
                    lfr.create_array(
                        array_shape, floating_point_element_type, sdp_factor_fill_value
                    ),
                ]

                yield_factors_per_crop = [
                    lfr.create_array(
                        array_shape,
                        floating_point_element_type,
                        yield_factor_fill_value,
                    ),
                    lfr.create_array(
                        array_shape,
                        floating_point_element_type,
                        yield_factor_fill_value,
                    ),
                    lfr.create_array(
                        array_shape,
                        floating_point_element_type,
                        yield_factor_fill_value,
                    ),
                ]

                crop_fractions = [
                    lfr.create_array(
                        array_shape,
                        floating_point_element_type,
                        crop_fraction_fill_value,
                    ),
                    lfr.create_array(
                        array_shape,
                        floating_point_element_type,
                        crop_fraction_fill_value,
                    ),
                    lfr.create_array(
                        array_shape,
                        floating_point_element_type,
                        crop_fraction_fill_value,
                    ),
                ]

                floating_point_array_type = type(crop_fractions[0])

                current_zonal_production_per_crop = lfr.make_ready_future(
                    {
                        zone: [production_fill_value for _ in range(nr_crops)]
                        for zone in zones
                    }
                )
                zonal_demand_per_crop = lfr.make_ready_future(
                    {
                        zone: [demand_fill_value for _ in range(nr_crops)]
                        for zone in zones
                    }
                )

                irrigated_crop_fractions = lfr.create_array(
                    array_shape, floating_point_element_type, crop_fraction_fill_value
                )

                (
                    crop_fractions_we_got,
                    zonal_production_we_got,
                ) = img.integrate_and_allocate(
                    route,
                    sdp_factors_per_crop,
                    yield_factors_per_crop,
                    crop_fractions,
                    zonal_demand_per_crop,
                    current_zonal_production_per_crop,
                    irrigated_crop_fractions,
                )

                self.assertTrue(isinstance(crop_fractions_we_got, list))
                self.assertEqual(len(crop_fractions_we_got), nr_crops)
                self.assertTrue(
                    all(
                        isinstance(element, floating_point_array_type)
                        for element in crop_fractions_we_got
                    )
                )

                self.assertTrue(
                    isinstance(
                        zonal_production_we_got, type(current_zonal_production_per_crop)
                    )
                )

                zonal_production_we_got = zonal_production_we_got.get()

                self.assertTrue(isinstance(zonal_production_we_got, dict))
                self.assertEqual(len(zonal_production_we_got), nr_zones)
                self.assertTrue(all(zone in zonal_production_we_got for zone in zones))

                for zone, production in zonal_production_we_got.items():
                    self.assertTrue(zone in zones)
                    self.assertTrue(isinstance(production, list))
                    self.assertEqual(len(production), nr_crops)
