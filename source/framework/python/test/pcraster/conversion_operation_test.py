import numpy as np

import lue.framework as lfr
import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class ConversionOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_boolean(self):
        for element_type in [int, float] + lfr.arithmetic_element_types:
            non_spatial = element_type(5)
            _ = lpr.boolean(non_spatial)

        for expression_type in lpr.arithmetic_element_types:
            spatial = self.spatial[expression_type]
            _ = lpr.boolean(spatial)

    @lue_test.framework_test_case
    def test_nominal(self):
        for expression_type in [int] + lfr.integral_element_types:
            non_spatial = expression_type(5)
            _ = lpr.nominal(non_spatial)

        for expression_type in [
            # TODO np.uint8,
            np.int32,
            # TODO np.float32,
        ]:
            spatial = self.spatial[expression_type]
            _ = lpr.nominal(spatial)

    @lue_test.framework_test_case
    def test_scalar(self):
        for expression_type in [int, float] + lfr.arithmetic_element_types:
            non_spatial = expression_type(5)
            _ = lpr.scalar(non_spatial)

        for expression_type in lpr.arithmetic_element_types:
            spatial = self.spatial[expression_type]
            _ = lpr.scalar(spatial)
