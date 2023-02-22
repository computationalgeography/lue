import itertools

import numpy as np

import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class GlobalOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_mapmaximum(self):
        for expression_type in [np.int32, np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.mapmaximum(expression)

    @lue_test.framework_test_case
    def test_mapminimum(self):
        for expression_type in [np.int32, np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.mapminimum(expression)
