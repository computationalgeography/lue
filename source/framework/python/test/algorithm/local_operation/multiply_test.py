import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class MultiplyTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for element_type in lfr.floating_point_element_types:
            value = self.value[element_type]
            scalar = self.scalar[element_type]
            array = self.array[element_type]

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
