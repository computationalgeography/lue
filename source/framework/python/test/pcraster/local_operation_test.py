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
        array_shape = (60, 40)
        partition_shape = (10, 10)

        cls.non_spatial = {
            np.float32: np.float32(3.2),
            np.float64: np.float64(6.4),
        }
        cls.spatial = {
            np.float32: lfr.create_array(
                array_shape, partition_shape, np.float32, fill_value=cls.non_spatial[np.float32]),
            np.float64: lfr.create_array(
                array_shape, partition_shape, np.float64, fill_value=cls.non_spatial[np.float64]),
        }

    @lue_test.framework_test_case
    def test_add(self):
        for type_ in [np.float32, np.float64]:
            spatial, non_spatial = self.spatial[type_], self.non_spatial[type_]

            _ = spatial + spatial
            _ = spatial + non_spatial
            _ = non_spatial + spatial
            _ = non_spatial + non_spatial

    @lue_test.framework_test_case
    def test_subtract(self):
        for type_ in [np.float32, np.float64]:
            spatial, non_spatial = self.spatial[type_], self.non_spatial[type_]

            _ = spatial - spatial
            _ = spatial - non_spatial
            _ = non_spatial - spatial
            _ = non_spatial - non_spatial

    @lue_test.framework_test_case
    def test_divide(self):
        for type_ in [np.float32, np.float64]:
            spatial, non_spatial = self.spatial[type_], self.non_spatial[type_]

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
        for type_ in [np.float32, np.float64]:
            spatial, non_spatial = self.spatial[type_], self.non_spatial[type_]

            _ = spatial * spatial
            _ = spatial * non_spatial
            _ = non_spatial * spatial
            _ = non_spatial * non_spatial

    @lue_test.framework_test_case
    def test_power(self):
        for type_ in [np.float32, np.float64]:
            spatial, non_spatial = self.spatial[type_], self.non_spatial[type_]

            _ = spatial ** spatial
            _ = spatial ** non_spatial
            _ = non_spatial ** spatial
            _ = non_spatial ** non_spatial

    @lue_test.framework_test_case
    def test_abs(self):
        for type_ in [np.float32, np.float64]:
            spatial, non_spatial = self.spatial[type_], self.non_spatial[type_]

            _ = lpr.abs(spatial)
            # TODO _ = lpr.abs(non_spatial)



