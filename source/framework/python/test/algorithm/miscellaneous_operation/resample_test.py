import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ResampleTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 3
        count = 5

        for element_type in lfr.arithmetic_element_types:
            source_array = lfr.create_array(array_shape, element_type, fill_value)

            self.assert_overload(
                lfr.downscale,
                source_array,
                count,
                lfr.DownscaleStrategy.assign,
            )
