import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ACosTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.floating_point_types:
            array = self.array[type_]
            scalar = self.scalar[type_]
            value = self.value[type_]

            _ = lfr.acos(array)
            _ = lfr.acos(scalar)
            _ = lfr.acos(value)
