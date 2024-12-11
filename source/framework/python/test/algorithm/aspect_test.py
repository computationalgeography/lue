import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AspectTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        fill_value = 5

        for element_type in lfr.floating_point_element_types:
            elevation = lfr.create_array(array_shape, element_type, fill_value)
            self.assert_overload(lfr.aspect, elevation)
