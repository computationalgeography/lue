import numpy as np

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class FocalHighPassTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        fill_value = 5
        kernel = np.full((3, 3), 1, dtype=lfr.boolean_element_type)

        for element_type in lfr.floating_point_element_types:
            array = lfr.create_array(array_shape, element_type, fill_value)
            self.assert_overload(lfr.focal_high_pass, array, kernel)
