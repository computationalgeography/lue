import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class LogicalNotTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.integral_types:
            array = self.array[type_]
            scalar = self.scalar[type_]
            value = self.value[type_]

            _ = lfr.logical_not(array)
            _ = lfr.logical_not(scalar)
            _ = lfr.logical_not(value)
