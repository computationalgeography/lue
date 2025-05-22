import numpy as np

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class UniformTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        min_value = 0
        max_value = 100

        for element_type in lfr.arithmetic_element_types:
            if element_type not in [np.int8, np.uint8]:
                self.assert_overload(lfr.uniform, element_type, min_value, max_value)
