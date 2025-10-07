import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AccuThresholdTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        flow_direction = self.array[lfr.flow_direction_element_type]

        for element_type in lfr.floating_point_element_types:
            inflow_array = self.array[element_type]
            inflow_scalar = self.scalar[element_type]

            threshold_array = self.array[element_type]
            threshold_scalar = self.scalar[element_type]

            self.assert_overload(
                lfr.accu_threshold,
                flow_direction,
                inflow_array,
                threshold_array,
            )
            self.assert_overload(
                lfr.accu_threshold,
                flow_direction,
                inflow_array,
                threshold_scalar,
            )
            self.assert_overload(
                lfr.accu_threshold,
                flow_direction,
                inflow_scalar,
                threshold_scalar,
            )
            self.assert_overload(
                lfr.accu_threshold,
                flow_direction,
                inflow_scalar,
                threshold_array,
            )
