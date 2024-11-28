import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ZonalAreaTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_zone = 3

        for element_type in lfr.zone_element_types:
            zones = lfr.create_array(array_shape, element_type, fill_zone)
            _ = lfr.zonal_area(zones)
