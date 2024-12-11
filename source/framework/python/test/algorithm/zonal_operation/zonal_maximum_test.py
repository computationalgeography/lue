import itertools

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ZonalMaximumTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5
        fill_zone = 3

        for value_element_type, zone_element_type in itertools.product(
            lfr.arithmetic_element_types,
            lfr.zone_element_types,
        ):
            array = lfr.create_array(array_shape, value_element_type, fill_value)
            zones = lfr.create_array(array_shape, zone_element_type, fill_zone)
            self.assert_overload(lfr.zonal_maximum, array, zones)
