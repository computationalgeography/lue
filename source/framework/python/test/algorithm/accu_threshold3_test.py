import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AccuThreshold3Test(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        for direction in [1, 2, 3, 4, 5, 6, 7, 8, 9]:
            flow_direction = lfr.create_array(
                array_shape, lfr.flow_direction_element_type, direction
            )

            for element_type in lfr.floating_point_element_types:
                external_inflow = lfr.create_array(array_shape, element_type, 1)
                threshold = lfr.create_array(array_shape, element_type, 5)

                self.assert_overload(
                    lfr.accu_threshold3, flow_direction, external_inflow, threshold
                )
