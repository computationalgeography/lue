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
            self.assert_overload(lfr.divide, array, array)
            self.assert_overload(lfr.divide, array, scalar)
            self.assert_overload(lfr.divide, array, value)
            self.assert_overload(lfr.divide, scalar, array)
            self.assert_overload(lfr.divide, value, array)

            # scalar icw something else, if not handled already
            self.assert_overload(lfr.divide, scalar, scalar)
            self.assert_overload(lfr.divide, scalar, value)
            self.assert_overload(lfr.divide, value, scalar)

            # value icw something else, if not handled already
            self.assert_overload(lfr.divide, value, value)
