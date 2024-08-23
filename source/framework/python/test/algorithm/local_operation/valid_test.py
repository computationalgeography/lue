import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ValidTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.numeric_types:
            value = self.value[type_]
            scalar = self.scalar[type_]
            array = self.array[type_]

            _ = lfr.valid(array)
            _ = lfr.valid(scalar)
            _ = lfr.valid(value)
