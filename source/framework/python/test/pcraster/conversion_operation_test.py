import numpy as np

import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class ConversionOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_boolean(self):

        for expression_type in [
            int,
            float,
            np.int32,
            np.int64,
            np.uint8,
            np.uint32,
            np.uint64,
            np.float32,
            np.float64,
        ]:
            non_spatial = expression_type(5)
            _ = lpr.boolean(non_spatial)

        for expression_type in [
            np.uint8,
            np.int32,
            np.float32,
        ]:
            spatial = self.spatial[expression_type]
            _ = lpr.boolean(spatial)

    @lue_test.framework_test_case
    def test_nominal(self):

        for expression_type in [
            int,
            float,
            np.int32,
            np.int64,
            np.uint8,
            np.uint32,
            np.uint64,
            np.float32,
            np.float64,
        ]:
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

        for expression_type in [
            int,
            float,
            np.int32,
            np.int64,
            np.uint8,
            np.uint32,
            np.uint64,
            np.float32,
            np.float64,
        ]:
            non_spatial = expression_type(5)
            _ = lpr.scalar(non_spatial)

        for expression_type in [
            np.uint8,
            np.int32,
            np.float32,
        ]:
            spatial = self.spatial[expression_type]
            _ = lpr.scalar(spatial)
