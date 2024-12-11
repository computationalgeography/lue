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
            self.assert_overload(lfr.multiply, array, array)
            self.assert_overload(lfr.multiply, array, scalar)
            self.assert_overload(lfr.multiply, array, value)
            self.assert_overload(lfr.multiply, scalar, array)
            self.assert_overload(lfr.multiply, value, array)

            # scalar icw something else, if not handled already
            self.assert_overload(lfr.multiply, scalar, scalar)
            self.assert_overload(lfr.multiply, scalar, value)
            self.assert_overload(lfr.multiply, value, scalar)

            # value icw something else, if not handled already
            self.assert_overload(lfr.multiply, value, value)
