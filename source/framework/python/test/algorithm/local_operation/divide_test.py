import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class DivideTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for element_type in lfr.floating_point_element_types:
            value = self.value[element_type]
            scalar = self.scalar[element_type]
            array = self.array[element_type]

            # array icw something else
            _ = lfr.divide(array, array)
            _ = lfr.divide(array, scalar)
            _ = lfr.divide(array, value)
            _ = lfr.divide(scalar, array)
            _ = lfr.divide(value, array)

            # scalar icw something else, if not handled already
            _ = lfr.divide(scalar, scalar)
            _ = lfr.divide(scalar, value)
            _ = lfr.divide(value, scalar)

            # value icw something else, if not handled already
            _ = lfr.divide(value, value)
