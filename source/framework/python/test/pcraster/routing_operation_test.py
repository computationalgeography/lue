import numpy as np

import lue.framework as lfr
import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class RoutingOperationTest(OperationTest):
    @lue_test.framework_test_case
    def test_accucapacity(self):
        ldd = self.ldd

        for type_ in [np.float32]:
            spatial_material, non_spatial_material = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            non_spatial_capacity = type_(0.5) * non_spatial_material
            spatial_capacity = lfr.create_array(
                self.array_shape,
                type_,
                non_spatial_capacity,
                partition_shape=self.partition_shape,
            )

            _ = lpr.accucapacityflux(ldd, spatial_material, spatial_capacity)
            _ = lpr.accucapacitystate(ldd, spatial_material, spatial_capacity)
            _ = lpr.accucapacityflux(ldd, non_spatial_material, spatial_capacity)
            _ = lpr.accucapacitystate(ldd, non_spatial_material, spatial_capacity)

    @lue_test.framework_test_case
    def test_accufraction(self):
        ldd = self.ldd

        for type_ in [np.float32]:
            spatial_material, non_spatial_material = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            # Fraction must be with [0, 1]
            non_spatial_fraction = (
                type_(0.5) * non_spatial_material / non_spatial_material
            )
            spatial_fraction = lfr.create_array(
                self.array_shape,
                type_,
                non_spatial_fraction,
                partition_shape=self.partition_shape,
            )

            _ = lpr.accufractionflux(ldd, spatial_material, spatial_fraction)
            _ = lpr.accufractionstate(ldd, spatial_material, spatial_fraction)
            _ = lpr.accufractionflux(ldd, non_spatial_material, non_spatial_fraction)
            _ = lpr.accufractionstate(ldd, non_spatial_material, non_spatial_fraction)

    @lue_test.framework_test_case
    def test_accuthreshold(self):
        ldd = self.ldd

        for type_ in [np.float32]:
            spatial_material, non_spatial_material = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            non_spatial_threshold = type_(0.5) * non_spatial_material
            spatial_threshold = lfr.create_array(
                self.array_shape,
                type_,
                non_spatial_threshold,
                partition_shape=self.partition_shape,
            )

            _ = lpr.accuthresholdflux(ldd, spatial_material, spatial_threshold)
            _ = lpr.accuthresholdstate(ldd, spatial_material, spatial_threshold)
            _ = lpr.accuthresholdflux(ldd, non_spatial_material, spatial_threshold)
            _ = lpr.accuthresholdstate(ldd, non_spatial_material, spatial_threshold)

    @lue_test.framework_test_case
    def test_accutrigger(self):
        ldd = self.ldd

        for type_ in [np.float32]:
            spatial_material, non_spatial_material = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            non_spatial_trigger = type_(0.5) * non_spatial_material
            spatial_trigger = lfr.create_array(
                self.array_shape,
                type_,
                non_spatial_trigger,
                partition_shape=self.partition_shape,
            )

            _ = lpr.accutriggerflux(ldd, spatial_material, spatial_trigger)
            _ = lpr.accutriggerstate(ldd, spatial_material, spatial_trigger)
            _ = lpr.accutriggerflux(ldd, non_spatial_material, spatial_trigger)
            _ = lpr.accutriggerstate(ldd, non_spatial_material, spatial_trigger)

    @lue_test.framework_test_case
    def test_downstream(self):
        ldd = self.ldd

        for type_ in [np.uint8, np.int32, np.float32]:
            spatial_expression = self.spatial[type_]

            _ = lpr.downstream(ldd, spatial_expression)

    @lue_test.framework_test_case
    def test_downstreamdist(self):
        ldd = self.ldd

        _ = lpr.downstreamdist(ldd)

    @lue_test.framework_test_case
    def test_kinematic(self):
        ldd = self.ldd

        for type_ in [np.float32]:
            spatial, non_spatial = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            _ = lpr.kinematic(
                ldd,
                spatial,
                spatial,
                spatial,
                spatial,
                non_spatial,  # <-- nr_time_slices, which is unused by LUE
                non_spatial,
                spatial,
            )
            _ = lpr.kinematic(
                ldd,
                spatial,
                spatial,
                non_spatial,
                non_spatial,
                non_spatial,  # <-- nr_time_slices, which is unused by LUE
                non_spatial,
                non_spatial,
            )

    @lue_test.framework_test_case
    def test_lddcreate(self):
        for type_ in [np.float32]:
            spatial, non_spatial = (
                self.spatial[type_],
                self.non_spatial[type_],
            )

            _ = lpr.lddcreate(
                spatial,
                non_spatial,
                non_spatial,
                non_spatial,
                non_spatial,
            )

    @lue_test.framework_test_case
    def test_upstream(self):
        ldd = self.ldd

        for type_ in [np.float32]:
            spatial, non_spatial = (
                self.spatial[type_],
                self.value[type_],  # TODO non_spatial (Scalar) is not supported yet
            )

            _ = lpr.upstream(ldd, non_spatial)
            _ = lpr.upstream(ldd, spatial)
