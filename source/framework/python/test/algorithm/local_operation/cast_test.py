import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class CastTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for input_type in self.numeric_types:
            value = self.value[input_type]
            scalar = self.scalar[input_type]
            array = self.array[input_type]

            for output_type in self.numeric_types:

                _ = lfr.cast(array, output_type)
                _ = lfr.cast(scalar, output_type)
                _ = lfr.cast(value, output_type)
