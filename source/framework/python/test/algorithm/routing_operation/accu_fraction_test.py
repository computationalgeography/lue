# import lue.framework as lfr
# import lue_test
# from lue_test.operation_test import OperationTest, setUpModule, tearDownModule
#
#
# class AccuFractionTest(OperationTest):
#     @lue_test.framework_test_case
#     def test_overloads(self):
#         array_shape = (60, 40)
#
#         direction = 2
#         flow_direction = lfr.create_array(
#             array_shape, lfr.flow_direction_element_type, direction
#         )
#
#         for element_type in lfr.floating_point_element_types:
#             material = lfr.create_array(array_shape, element_type, 1)
#             fraction = lfr.create_array(array_shape, element_type, 0.8)
#             self.assert_overload(lfr.accu_fraction, flow_direction, material, fraction)


import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AccuFractionTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        flow_direction = self.array[lfr.flow_direction_element_type]

        for element_type in lfr.floating_point_element_types:
            inflow_array = self.array[element_type]
            inflow_scalar = self.scalar[element_type]

            fraction_array = self.array[element_type]
            fraction_scalar = self.scalar[element_type]

            self.assert_overload(
                lfr.accu_fraction,
                flow_direction,
                inflow_array,
                fraction_array,
            )
            self.assert_overload(
                lfr.accu_fraction,
                flow_direction,
                inflow_array,
                fraction_scalar,
            )
            self.assert_overload(
                lfr.accu_fraction,
                flow_direction,
                inflow_scalar,
                fraction_scalar,
            )
            self.assert_overload(
                lfr.accu_fraction,
                flow_direction,
                inflow_scalar,
                fraction_array,
            )
