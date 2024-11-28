import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AddTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for element_type in lfr.arithmetic_element_types:
            value = self.value[element_type]
            scalar = self.scalar[element_type]
            array = self.array[element_type]

            # array icw something else
            _ = lfr.add(array, array)
            _ = lfr.add(array, scalar)
            _ = lfr.add(array, value)
            _ = lfr.add(scalar, array)
            _ = lfr.add(value, array)

            # scalar icw something else, if not handled already
            _ = lfr.add(scalar, scalar)
            _ = lfr.add(scalar, value)
            _ = lfr.add(value, scalar)

            # value icw something else, if not handled already
            _ = lfr.add(value, value)
