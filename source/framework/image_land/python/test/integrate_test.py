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

        for zone_element_type in lfr.zone_element_types:
            for integrand_element_type in lfr.floating_point_element_types:
                zone = lfr.create_array(array_shape, zone_element_type, zone_fill_value)
                field = lfr.create_array(
                    array_shape, integrand_element_type, integrand_fill_value
                )
                integrand = lfr.create_array(
                    array_shape, integrand_element_type, integrand_fill_value
                )
                route = lfr.decreasing_order(zone, field, max_nr_cells=max_nr_cells)

                img.integrate(route, integrand, max_nr_cells=max_nr_cells)
