import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AccuInfoTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        flow_direction = self.array[lfr.flow_direction_element_type]

        self.assert_overload(lfr.accu_info, flow_direction)
