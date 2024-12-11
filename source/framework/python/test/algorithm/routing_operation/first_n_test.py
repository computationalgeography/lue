import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class FirstNTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_zone = 3
        fill_value = 5.5
        max_nr_cells = 10

        for zone_element_type in lfr.zone_element_types:
            values = lfr.create_array(
                array_shape, lfr.floating_point_element_types[0], fill_value
            )
            zones = lfr.create_array(array_shape, zone_element_type, fill_zone)
            route = lfr.decreasing_order(zones, values)

            # TODO
            # RuntimeError: this promise has no valid shared state: HPX(no_state)
            # https://github.com/computationalgeography/lue/issues/629
            # self.assert_overload(lfr.first_n, route, max_nr_cells)
