import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class RoundTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for element_type in lfr.floating_point_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            _ = lfr.round(array)
            _ = lfr.round(scalar)
            _ = lfr.round(value)
