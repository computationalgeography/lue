import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class OpenSimplex2NoiseTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        condition = lfr.create_array(array_shape, lfr.boolean_element_type, 1)
        seed = 5

        for float_element_type in lfr.floating_point_element_types:
            x_coordinates = lfr.cast(lfr.cell_index(condition, 1), float_element_type)
            y_coordinates = lfr.cast(lfr.cell_index(condition, 0), float_element_type)

            self.assert_overload(
                lfr.open_simplex_noise, x_coordinates, y_coordinates, seed
            )
