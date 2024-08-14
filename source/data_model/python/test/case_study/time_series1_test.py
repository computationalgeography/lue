import numpy

import lue.data_model as ldm
import lue_test


class TimeSeriesTest(lue_test.TestCase):
    def test_case_study(self):
        # Time series as implemented here:
        # - Discharge at catchment outlets
        #     - Located at fixed points in space
        #     - Variable number of outlets per time box
        #     - Discretized within multiple time boxes

        # - Time domain contains time boxes
        # - Space domain contains space points
        # - Property values are same_shape::different_shape (shape of value is
        #     related to what is stored per box and boxes likely have different
        #     numbers of cells)
        # - Property values are discretized
        # - Per time box the set of active objects is tracked
        # - Per time box active objects exist in all its cells
        # - Use this approach if the active set is constant within a time box

        nr_time_boxes = 3
        shape_datatype = ldm.dtype.Count
        shapes = numpy.random.randint(10, 20, size=nr_time_boxes).astype(
            dtype=shape_datatype
        )

        def discharge_property(phenomenon):
            nr_outlets = 10

            ids = numpy.arange(nr_outlets, dtype=numpy.uint64)
            phenomenon.object_id.expand(nr_outlets)[:] = ids

            # Time domain
            time_configuration = ldm.TimeConfiguration(ldm.TimeDomainItemType.box)
            clock = ldm.Clock(ldm.Unit.day, 1)

            # Space domain
            space_configuration = ldm.SpaceConfiguration(
                ldm.Mobility.stationary, ldm.SpaceDomainItemType.point
            )
            space_coordinate_datatype = numpy.float64
            rank = 2

            # Property set
            outlet_points = phenomenon.add_property_set(
                "outlets",
                time_configuration,
                clock,
                space_configuration,
                space_coordinate_datatype,
                rank,
            )

            # IDs
            object_tracker = outlet_points.object_tracker

            object_tracker.active_set_index.expand(nr_time_boxes)

            active_set_sizes = numpy.random.randint(
                0, nr_outlets, nr_time_boxes
            ).astype(dtype=ldm.dtype.Count)
            active_set_idxs = numpy.empty(nr_time_boxes, dtype=ldm.dtype.Index)
            active_object_ids = numpy.empty(active_set_sizes.sum(), dtype=ldm.dtype.ID)

            ldm.test.select_random_ids(
                active_set_sizes, active_set_idxs, active_object_ids, nr_outlets
            )

            object_tracker.active_object_id.expand(len(active_object_ids))

            active_set_idx = numpy.uint64(0)

            for s in range(nr_time_boxes):
                active_set_size = active_set_sizes[s]
                object_tracker.active_set_index[s] = active_set_idxs[s]
                object_tracker.active_object_id[
                    active_set_idx : active_set_idx + active_set_size
                ] = active_object_ids[active_set_idx : active_set_idx + active_set_size]
                active_set_idx += active_set_size

            # Time domain
            time_domain = outlet_points.time_domain
            time_coordinate_datatype = ldm.dtype.TickPeriodCount
            time_boxes = numpy.arange(
                nr_time_boxes * 2, dtype=time_coordinate_datatype
            ).reshape(nr_time_boxes, 2)
            time_domain.value.expand(nr_time_boxes)[:] = time_boxes

            # Space domain
            space_domain = outlet_points.space_domain
            space_points = numpy.arange(
                nr_outlets * rank, dtype=space_coordinate_datatype
            ).reshape(nr_outlets, 2)
            space_domain.value.expand(nr_outlets)[:] = space_points

            # Property
            discharge_datatype = numpy.float32
            discharge = outlet_points.add_property(
                "discharge",
                dtype=discharge_datatype,
                rank=1,
                shape_per_object=ldm.ShapePerObject.same,
                shape_variability=ldm.ShapeVariability.variable,
            )

            discharge_values = [
                numpy.arange(shapes[t] * active_set_sizes[t], dtype=discharge_datatype)
                for t in range(nr_time_boxes)
            ]

            for t in range(nr_time_boxes):
                discharge.value.expand(t, active_set_sizes[t], (shapes[t],))[:] = (
                    discharge_values[t]
                )

            return discharge

        def discretization_property(phenomenon):
            # Property set
            collection = phenomenon.add_collection_property_set(
                "outlets_collection", phenomenon.property_sets["outlets"].time_domain
            )

            # IDs
            object_tracker = collection.object_tracker
            object_tracker.active_set_index.expand(nr_time_boxes)
            object_tracker.active_object_id.expand(nr_time_boxes)

            collection_id = 5
            active_set_idx = 0

            for s in range(nr_time_boxes):
                active_set_size = 1
                object_tracker.active_set_index[s] = active_set_idx
                object_tracker.active_object_id[
                    active_set_idx : active_set_idx + active_set_size
                ] = collection_id
                active_set_idx += active_set_size

            # Property
            discretization = collection.add_property(
                "discretization",
                dtype=shape_datatype,
                shape=(1,),
                value_variability=ldm.ValueVariability.variable,
            )
            discretization.value.expand(nr_time_boxes)[:] = shapes

            return discretization

        dataset = ldm.create_dataset("outlets1.lue")
        phenomenon = dataset.add_phenomenon("areas")
        discharge = discharge_property(phenomenon)
        discretization = discretization_property(phenomenon)

        # Link discharge to discretization
        discharge.set_time_discretization(
            ldm.TimeDiscretization.regular_grid, discretization
        )

        ldm.assert_is_valid(dataset)
