import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AccuTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        flow_direction = self.array[lfr.flow_direction_element_type]

        for element_type in lfr.floating_point_element_types:
            inflow_array = self.array[element_type]
            inflow_scalar = self.scalar[element_type]
            inflow_value = self.value[element_type]

            self.assert_overload(lfr.accu, flow_direction, inflow_array)
            self.assert_overload(lfr.accu, flow_direction, inflow_scalar)
            self.assert_overload(lfr.accu, flow_direction, inflow_value)
