import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class InflowCount3Test(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        for direction in [1, 2, 3, 4, 5, 6, 7, 8, 9]:
            flow_direction = lfr.create_array(
                array_shape, lfr.flow_direction_element_type, direction
            )
            self.assert_overload(lfr.inflow_count3, flow_direction)
