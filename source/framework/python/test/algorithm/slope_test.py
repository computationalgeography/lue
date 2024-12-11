import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class SlopeTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        fill_value = 5
        cell_size = 25

        for element_type in lfr.floating_point_element_types:
            elevation = lfr.create_array(array_shape, element_type, fill_value)
            self.assert_overload(lfr.slope, elevation, cell_size)
