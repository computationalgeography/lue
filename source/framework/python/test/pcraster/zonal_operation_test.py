import lue.framework as lfr
import lue.pcraster as lpr
import lue_test
import numpy as np
import itertools


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ZonalOperationTest(lue_test.TestCase):
    @classmethod
    @lue_test.framework_test_case
    def setUpClass(cls):
        cls.array_shape = (60, 40)
        cls.partition_shape = (10, 10)

        # TODO Can we do better?
        # - When the pcraster module must create spatials from non-spatials, this
        #   information currently must be passed in
        lpr.configuration.array_shape = cls.array_shape
        lpr.configuration.partition_shape = cls.partition_shape

        cls.non_spatial = {
            np.uint8: np.uint8(1),
            np.uint32: np.uint32(32),
            np.uint64: np.uint32(64),
            np.int32: np.int32(-32),
            np.int64: np.int32(-64),
            np.float32: np.float32(3.2),
            np.float64: np.float64(6.4),
        }
        cls.spatial = {
            np.uint8: lfr.create_array(
                cls.array_shape,
                cls.partition_shape,
                np.uint8,
                fill_value=cls.non_spatial[np.uint8],
            ),
            np.uint32: lfr.create_array(
                cls.array_shape,
                cls.partition_shape,
                np.uint32,
                fill_value=cls.non_spatial[np.uint32],
            ),
            np.uint64: lfr.create_array(
                cls.array_shape,
                cls.partition_shape,
                np.uint64,
                fill_value=cls.non_spatial[np.uint64],
            ),
            np.int32: lfr.create_array(
                cls.array_shape,
                cls.partition_shape,
                np.int32,
                fill_value=cls.non_spatial[np.int32],
            ),
            np.int64: lfr.create_array(
                cls.array_shape,
                cls.partition_shape,
                np.int64,
                fill_value=cls.non_spatial[np.int64],
            ),
            np.float32: lfr.create_array(
                cls.array_shape,
                cls.partition_shape,
                np.float32,
                fill_value=cls.non_spatial[np.float32],
            ),
            np.float64: lfr.create_array(
                cls.array_shape,
                cls.partition_shape,
                np.float64,
                fill_value=cls.non_spatial[np.float64],
            ),
        }

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
