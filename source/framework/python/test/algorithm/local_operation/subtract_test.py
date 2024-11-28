import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class SubtractTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for element_type in lfr.arithmetic_element_types:
            value = self.value[element_type]
            scalar = self.scalar[element_type]
            array = self.array[element_type]

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
