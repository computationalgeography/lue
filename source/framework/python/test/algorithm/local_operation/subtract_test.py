import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class SubtractTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.numeric_types:
            value = self.value[type_]
            scalar = self.scalar[type_]
            array = self.array[type_]

            # array icw something else
            _ = lfr.subtract(array, array)
            _ = lfr.subtract(array, scalar)
            _ = lfr.subtract(array, value)
            _ = lfr.subtract(scalar, array)
            _ = lfr.subtract(value, array)

            # scalar icw something else, if not handled already
            _ = lfr.subtract(scalar, scalar)
            _ = lfr.subtract(scalar, value)
            _ = lfr.subtract(value, scalar)

            # value icw something else, if not handled already
            _ = lfr.subtract(value, value)
