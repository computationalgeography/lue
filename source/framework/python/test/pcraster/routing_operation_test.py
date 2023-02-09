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
        lpr.configuration.partition_shape = cls.partition_shape

        cls.non_spatial = {
            np.float32: np.float32(3.2),
            np.float64: np.float64(6.4),
        }
        cls.spatial = {
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
        direction = 3
        cls.ldd = lfr.create_array(
            cls.array_shape, cls.partition_shape, np.uint8, direction
        )

    @lue_test.framework_test_case
    def test_accufraction(self):
        ldd = self.ldd

        for type_ in [np.float32, np.float64]:
            spatial_material, non_spatial_material = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            # Fraction must be with [0, 1]
            non_spatial_fraction = (
                type_(0.5) * non_spatial_material / non_spatial_material
            )
            spatial_fraction = lfr.create_array(
                self.array_shape, self.partition_shape, type_, non_spatial_fraction
            )

            _ = lpr.accufractionflux(ldd, spatial_material, spatial_fraction)
            _ = lpr.accufractionstate(ldd, spatial_material, spatial_fraction)
            _ = lpr.accufractionflux(ldd, non_spatial_material, non_spatial_fraction)
            _ = lpr.accufractionstate(ldd, non_spatial_material, non_spatial_fraction)

    @lue_test.framework_test_case
    def test_accuthreshold(self):
        ldd = self.ldd

        for type_ in [np.float32, np.float64]:
            spatial_material, non_spatial_material = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            non_spatial_threshold = type_(0.5) * non_spatial_material
            spatial_threshold = lfr.create_array(
                self.array_shape, self.partition_shape, type_, non_spatial_threshold
            )

            _ = lpr.accuthresholdflux(ldd, spatial_material, spatial_threshold)
            _ = lpr.accuthresholdstate(ldd, spatial_material, spatial_threshold)
            _ = lpr.accuthresholdflux(ldd, non_spatial_material, spatial_threshold)
            _ = lpr.accuthresholdstate(ldd, non_spatial_material, spatial_threshold)

    # Hier verder, ander accu functies
