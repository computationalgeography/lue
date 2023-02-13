import lue.framework as lfr
import lue.pcraster as lpr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class LocalOperationTest(lue_test.TestCase):
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
    def test_add(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial + spatial
            _ = spatial + non_spatial
            _ = non_spatial + spatial
            _ = non_spatial + non_spatial

    @lue_test.framework_test_case
    def test_subtract(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial - spatial
            _ = spatial - non_spatial
            _ = non_spatial - spatial
            _ = non_spatial - non_spatial

    @lue_test.framework_test_case
    def test_divide(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial / spatial
            _ = spatial / non_spatial
            _ = non_spatial / spatial
            _ = non_spatial / non_spatial

            _ = lpr.div(spatial, spatial)
            _ = lpr.div(spatial, non_spatial)
            _ = lpr.div(non_spatial, spatial)
            _ = lpr.div(non_spatial, non_spatial)

    @lue_test.framework_test_case
    def test_multiply(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial * spatial
            _ = spatial * non_spatial
            _ = non_spatial * spatial
            _ = non_spatial * non_spatial

    @lue_test.framework_test_case
    def test_power(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = spatial**spatial
            _ = spatial**non_spatial
            _ = non_spatial**spatial
            _ = non_spatial**non_spatial

    @lue_test.framework_test_case
    def test_abs(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.abs(spatial)
            _ = lpr.abs(non_spatial)

    @lue_test.framework_test_case
    def test_acos(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.acos(spatial)
            _ = lpr.acos(non_spatial)

    @lue_test.framework_test_case
    def test_and(self):
        for expression_type in [np.uint8]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.pcrand(spatial, spatial)
            _ = lpr.pcrand(spatial, spatial)
            _ = lpr.pcrand(non_spatial, non_spatial)
            _ = lpr.pcrand(non_spatial, non_spatial)

            _ = spatial & spatial
            _ = spatial & spatial
            _ = non_spatial & non_spatial
            _ = non_spatial & non_spatial

    @lue_test.framework_test_case
    def test_asin(self):
        for expression_type in [np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            _ = lpr.asin(spatial)
            _ = lpr.asin(non_spatial)

    # TODO Result in an int32 array, not uint8 array
    # @lue_test.framework_test_case
    # def test_boolean(self):
    #     for expression_type in [np.uint8, np.int32, np.float32]:
    #         spatial, non_spatial = self.spatial[expression_type], self.non_spatial[expression_type]

    #         _ = lpr.boolean(spatial)
    #         _ = lpr.boolean(non_spatial)
