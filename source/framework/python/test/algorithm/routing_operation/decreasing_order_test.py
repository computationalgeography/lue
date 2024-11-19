import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class DecreasingOrderTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5
        fill_zone = 3
        max_nr_cells = 10

        for value_element_type in lfr.arithmetic_element_types:
            values = lfr.create_array(array_shape, value_element_type, fill_value)

            lfr.decreasing_order(values)
            lfr.decreasing_order(values, max_nr_cells=max_nr_cells)

            for zone_element_type in lfr.zone_element_types:
                zones = lfr.create_array(array_shape, zone_element_type, fill_zone)

                lfr.decreasing_order(zones, values)
                lfr.decreasing_order(zones, values, max_nr_cells=max_nr_cells)
