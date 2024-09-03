import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ATanTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.floating_point_types:
            array = self.array[type_]
            scalar = self.scalar[type_]
            value = self.value[type_]

            _ = lfr.atan(array)
            _ = lfr.atan(scalar)
            _ = lfr.atan(value)
