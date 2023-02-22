import itertools

import numpy as np

import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class FocalOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_aspect(self):
        for dem_type in [np.float32]:
            dem = self.spatial[dem_type]

            _ = lpr.aspect(dem)

    @lue_test.framework_test_case
    def test_slope(self):
        for dem_type in [np.float32]:
            dem = self.spatial[dem_type]

            _ = lpr.slope(dem)

    @lue_test.framework_test_case
    def test_window4total(self):
        for expression_type in [np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.window4total(expression)

    @lue_test.framework_test_case
    def test_windowaverage(self):
        window_length = 3 * lpr.configuration.cell_size

        for expression_type in [np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.windowaverage(expression, window_length)

    @lue_test.framework_test_case
    def test_windowdiversity(self):
        window_length = 3 * lpr.configuration.cell_size

        for expression_type in [np.uint8, np.int32]:
            expression = self.spatial[expression_type]

            _ = lpr.windowdiversity(expression, window_length)

    @lue_test.framework_test_case
    def test_windowhighpass(self):
        window_length = 3 * lpr.configuration.cell_size

        for expression_type in [np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.windowhighpass(expression, window_length)

    @lue_test.framework_test_case
    def test_windowmajority(self):
        window_length = 3 * lpr.configuration.cell_size

        for expression_type in [np.uint8, np.int32]:
            expression = self.spatial[expression_type]

            _ = lpr.windowmajority(expression, window_length)

    @lue_test.framework_test_case
    def test_windowmaximum(self):
        window_length = 3 * lpr.configuration.cell_size

        for expression_type in [np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.windowmaximum(expression, window_length)

    @lue_test.framework_test_case
    def test_windowminimum(self):
        window_length = 3 * lpr.configuration.cell_size

        for expression_type in [np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.windowminimum(expression, window_length)

    @lue_test.framework_test_case
    def test_windowtotal(self):
        window_length = 3 * lpr.configuration.cell_size

        for expression_type in [np.float32]:
            expression = self.spatial[expression_type]

            _ = lpr.windowtotal(expression, window_length)
