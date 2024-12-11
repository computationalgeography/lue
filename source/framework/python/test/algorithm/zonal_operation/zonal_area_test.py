import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ZonalAreaTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_zone = 3

        for element_type in lfr.zone_element_types:
            zones = lfr.create_array(array_shape, element_type, fill_zone)
            self.assert_overload(lfr.zonal_area, zones)
