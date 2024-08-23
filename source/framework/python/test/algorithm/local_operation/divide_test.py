import numpy as np

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class DivideTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.floating_point_types:
            value = self.value[type_]
            scalar = self.scalar[type_]
            array = self.array[type_]

            # array icw something else
            _ = lfr.divide(array, array)
            _ = lfr.divide(array, scalar)
            _ = lfr.divide(array, value)
            _ = lfr.divide(scalar, array)
            _ = lfr.divide(value, array)

            # scalar icw something else, if not handled already
            _ = lfr.divide(scalar, scalar)
            _ = lfr.divide(scalar, value)
            _ = lfr.divide(value, scalar)

            # value icw something else, if not handled already
            _ = lfr.divide(value, value)
