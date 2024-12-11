import unittest

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


@unittest.skipIf(not hasattr(lfr, "locality_id"), "locality_id is not built-in")
class LocalityIDTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        for element_type in lfr.arithmetic_element_types:
            array = lfr.create_array(array_shape, element_type, 0)
            self.assert_overload(lfr.locality_id, array)
