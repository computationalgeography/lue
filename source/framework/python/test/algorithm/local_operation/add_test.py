import numpy as np

import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class AddTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.numeric_types:
            value = self.value[type_]
            scalar = self.scalar[type_]
            array = self.array[type_]

            # array icw something else
            _ = lfr.add(array, array)
            _ = lfr.add(array, scalar)
            _ = lfr.add(array, value)
            _ = lfr.add(scalar, array)
            _ = lfr.add(value, array)

            # scalar icw something else, if not handled already
            _ = lfr.add(scalar, scalar)
            _ = lfr.add(scalar, value)
            _ = lfr.add(value, scalar)

            # value icw something else, if not handled already
            _ = lfr.add(value, value)
