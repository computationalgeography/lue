import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AccuInfoTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        direction = 4
        flow_direction = lfr.create_array(
            array_shape, lfr.flow_direction_element_type, direction
        )
        self.assert_overload(lfr.accu_info3, flow_direction)
