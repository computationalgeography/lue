import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AbsTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in set(self.numeric_types) - set(self.unsigned_integral_types):
            array = self.array[type_]
            scalar = self.scalar[type_]
            value = self.value[type_]

            _ = lfr.abs(array)
            _ = lfr.abs(scalar)
            _ = lfr.abs(value)
