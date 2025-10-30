import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class KinematicWaveTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        flow_direction = self.array[lfr.flow_direction_element_type]

        for element_type in lfr.floating_point_element_types:
            current_outflow = self.array[element_type]
            inflow = self.array[element_type]
            alpha_array = self.array[element_type]
            alpha_scalar = self.scalar[element_type]
            beta_array = self.array[element_type]
            beta_scalar = self.scalar[element_type]
            time_step_duration_scalar = self.scalar[element_type]
            channel_length_array = self.array[element_type]
            channel_length_scalar = self.scalar[element_type]

            self.assert_overload(
                lfr.kinematic_wave,
                flow_direction,
                current_outflow,
                inflow,
                alpha_array,
                beta_array,
                time_step_duration_scalar,
                channel_length_array,
            )
            self.assert_overload(
                lfr.kinematic_wave,
                flow_direction,
                current_outflow,
                inflow,
                alpha_scalar,
                beta_scalar,
                time_step_duration_scalar,
                channel_length_scalar,
            )
