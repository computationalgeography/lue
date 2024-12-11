import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class LogicalNotTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for element_type in lfr.integral_element_types:
            array = self.array[element_type]
            scalar = self.scalar[element_type]
            value = self.value[element_type]

            self.assert_overload(lfr.logical_not, array)
            self.assert_overload(lfr.logical_not, scalar)
            self.assert_overload(lfr.logical_not, value)
