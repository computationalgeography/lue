import unittest

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


@unittest.skipIf(not hasattr(lfr, "timestamp"), "timestamp is not built-in")
class TimestampTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        if lfr.with_development_operations:
            array_shape = (60, 40)

            for element_type in lfr.arithmetic_element_types:
                array = lfr.create_array(array_shape, element_type, 0)
                self.assert_overload(lfr.timestamp, array)
