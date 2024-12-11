import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ValidTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for element_type in lfr.arithmetic_element_types:
            value = self.value[element_type]
            scalar = self.scalar[element_type]
            array = self.array[element_type]

            self.assert_overload(lfr.valid, array)
            self.assert_overload(lfr.valid, scalar)
            self.assert_overload(lfr.valid, value)
