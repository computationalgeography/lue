import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class DownstreamTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        direction = 1
        flow_direction = lfr.create_array(
            array_shape, lfr.flow_direction_element_type, direction
        )

        for element_type in lfr.material_element_types:
            material = lfr.create_array(array_shape, element_type, 1)
            self.assert_overload(lfr.downstream, flow_direction, material)
