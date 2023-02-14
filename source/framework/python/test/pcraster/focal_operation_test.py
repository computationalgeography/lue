import lue.framework as lfr
import lue.pcraster as lpr
import lue_test
import numpy as np
import itertools
from .operation_test import OperationTest, setUpModule, tearDownModule


class FocalOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_aspect(self):
        for dem_type in [np.float32]:
            dem = self.spatial[dem_type]

            _ = lpr.aspect(dem)
