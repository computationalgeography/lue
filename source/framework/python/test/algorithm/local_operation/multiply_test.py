import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class MultiplyTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.floating_point_types:
            value = self.value[type_]
            scalar = self.scalar[type_]
            array = self.array[type_]

            # array icw something else
            _ = lfr.multiply(array, array)
            _ = lfr.multiply(array, scalar)
            _ = lfr.multiply(array, value)
            _ = lfr.multiply(scalar, array)
            _ = lfr.multiply(value, array)

            # scalar icw something else, if not handled already
            _ = lfr.multiply(scalar, scalar)
            _ = lfr.multiply(scalar, value)
            _ = lfr.multiply(value, scalar)

            # value icw something else, if not handled already
            _ = lfr.multiply(value, value)
