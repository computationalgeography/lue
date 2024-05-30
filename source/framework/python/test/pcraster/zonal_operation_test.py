import itertools

import numpy as np

import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class ZonalOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_areaarea(self):
        for areaclass_type in [np.uint8, np.int32]:
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areaarea(areaclass)

    @lue_test.framework_test_case
    def test_areaaverage(self):
        for expression_type, areaclass_type in itertools.product(
            (np.float32,),
            (np.uint8, np.int32),
        ):
            expression = self.spatial[expression_type]
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areaaverage(expression, areaclass)

    @lue_test.framework_test_case
    def test_areadiversity(self):
        for expression_type, areaclass_type in itertools.product(
            (np.uint8, np.int32),
            (np.uint8, np.int32),
        ):
            expression = self.spatial[expression_type]
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areadiversity(expression, areaclass)

    @lue_test.framework_test_case
    def test_areamajority(self):
        for expression_type, areaclass_type in itertools.product(
            (np.uint8, np.int32),
            (np.uint8, np.int32),
        ):
            expression = self.spatial[expression_type]
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areamajority(expression, areaclass)

    @lue_test.framework_test_case
    def test_areamaximum(self):
        for expression_type, areaclass_type in itertools.product(
            (np.int32, np.float32),
            (np.uint8, np.int32),
        ):
            expression = self.spatial[expression_type]
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areamaximum(expression, areaclass)

    @lue_test.framework_test_case
    def test_areaminimum(self):
        for expression_type, areaclass_type in itertools.product(
            (np.int32, np.float32),
            (np.uint8, np.int32),
        ):
            expression = self.spatial[expression_type]
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areaminimum(expression, areaclass)

    @lue_test.framework_test_case
    def test_areanormal(self):
        for areaclass_type in [np.uint8, np.int32]:
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areanormal(areaclass)

    @lue_test.framework_test_case
    def test_areatotal(self):
        for expression_type, areaclass_type in itertools.product(
            (np.float32,),
            (np.uint8, np.int32),
        ):
            expression = self.spatial[expression_type]
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areatotal(expression, areaclass)

    @lue_test.framework_test_case
    def test_areauniform(self):
        for areaclass_type in [np.uint8, np.int32]:
            areaclass = self.spatial[areaclass_type]

            _ = lpr.areauniform(areaclass)

    @lue_test.framework_test_case
    def test_clump(self):
        for areaclass_type in [np.uint8, np.int32]:
            areaclass = self.spatial[areaclass_type]

            _ = lpr.clump(areaclass)
