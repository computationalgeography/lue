import numpy as np

import lue.framework as lfr
import lue_test

from ..operation_test import OperationTest, setUpModule, tearDownModule


class AddTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):

        for type_ in self.numeric_types:
            value = self.value[type_]
            scalar = self.scalar[type_]
            array = self.array[type_]

            # value icw scalar
            _ = lfr.add(value, scalar)
            _ = lfr.add(scalar, value)

            _ = value + scalar
            _ = scalar + value

            # value icw array
            _ = lfr.add(value, array)
            _ = lfr.add(array, value)

            _ = value + array
            _ = array + value

            # scalar icw array
            _ = lfr.add(scalar, array)
            _ = lfr.add(array, scalar)

            _ = scalar + array
            _ = array + scalar
