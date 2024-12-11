import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class CastTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for input_element_type in lfr.arithmetic_element_types:
            value = self.value[input_element_type]
            scalar = self.scalar[input_element_type]
            array = self.array[input_element_type]

            for output_element_types in lfr.arithmetic_element_types:

                self.assert_overload(lfr.cast, array, output_element_types)
                self.assert_overload(lfr.cast, scalar, output_element_types)
                self.assert_overload(lfr.cast, value, output_element_types)
