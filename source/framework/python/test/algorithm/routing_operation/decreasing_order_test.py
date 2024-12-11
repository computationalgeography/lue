import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class DecreasingOrderTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5
        fill_zone = 3
        max_nr_cells = 10

        for value_element_type in lfr.arithmetic_element_types:
            values = lfr.create_array(array_shape, value_element_type, fill_value)

            self.assert_overload(lfr.decreasing_order, values)
            self.assert_overload(
                lfr.decreasing_order, values, max_nr_cells=max_nr_cells
            )

            for zone_element_type in lfr.zone_element_types:
                zones = lfr.create_array(array_shape, zone_element_type, fill_zone)

                self.assert_overload(lfr.decreasing_order, zones, values)
                self.assert_overload(
                    lfr.decreasing_order, zones, values, max_nr_cells=max_nr_cells
                )
