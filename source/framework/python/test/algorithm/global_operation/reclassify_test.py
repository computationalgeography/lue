import numpy as np

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ReclassifyTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        id_ = 3

        for from_element_type in lfr.integral_element_types:
            ids = lfr.create_array(array_shape, from_element_type, id_)

            for to_element_type in lfr.arithmetic_element_types:
                lookup_table = {
                    1: 4,
                    2: 3,
                    3: 2,
                    4: 1,
                }
                array = lfr.reclassify(ids, lookup_table, dtype=to_element_type)

                self.assertEqual(array.dtype, np.dtype(to_element_type))
