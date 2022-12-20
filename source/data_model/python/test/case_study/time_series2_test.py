import lue.data_model as ldm
import lue_test
import numpy
import random


class TimeSeriesTest(lue_test.TestCase):

    def test_case_study(self):

        # Time series as implemented here:
        # - Discharge at catchment outlets
        #     - Located at fixed points in space
        #     - Variable number of outlets per time cell
        #     - Presence of outlets is discretized within multiple time boxes

        # - Time domain contains time cells
        # - Space domain contains space points
        # - Property values are same_shape::constant_shape (shape of value is
        #     related to what is stored per cell)
        # - Property values are not discretized
        # - Per time cell the set of active objects is tracked
        # - Use this approach if the active set is variable within a
        #     time box
        #     - - Additional storage required for tracking active sets,
        #         compared to Time series I
        #     - + Possible to let objects be 'born' and 'die' during
        #         iterative simulation

        dataset = ldm.create_dataset("outlets2.lue")
        phenomenon = dataset.add_phenomenon("areas")

        # Assume we are simulating some temporal variable (discharge at
        # catchment outlets).
        # The existance of the objects is modelled using time cells,
        # which are discretized time boxes (daily time steps). Per cell we
        # can store which objects are active.
        # Property values are located in time at time cells.
        # Property values are located in space at stationary space points.

        # Time domain
        time_configuration = ldm.TimeConfiguration(
            ldm.TimeDomainItemType.cell
        )
        epoch = ldm.Epoch(
            ldm.Epoch.Kind.common_era, "2019-01-01", ldm.Calendar.gregorian)
        clock = ldm.Clock(epoch, ldm.Unit.day, 1)
        time_coordinate_datatype = ldm.dtype.TickPeriodCount

        # Space domain
        space_configuration = ldm.SpaceConfiguration(
            ldm.Mobility.stationary,
            ldm.SpaceDomainItemType.point
        )
        space_coordinate_datatype = numpy.float32
        rank = 2

        # Property set
        outlet_points = phenomenon.add_property_set(
            "outlets",
            time_configuration, clock,
            space_configuration, space_coordinate_datatype, rank)
        time_domain = outlet_points.time_domain
        space_domain = outlet_points.space_domain
        active_set_index = outlet_points.object_tracker.active_set_index
        active_object_id = outlet_points.object_tracker.active_object_id

        # Property
        discharge_datatype = numpy.float32
        discharge = outlet_points.add_property(
            "discharge", dtype=discharge_datatype, shape=(1,),
            value_variability=ldm.ValueVariability.variable)

        nr_time_boxes = 5
        max_nr_objects = 100

        # Iterate over the time boxes
        for t in range(nr_time_boxes):

            # Store additional time box and count
            time_box = numpy.array([t, t + 1], dtype=time_coordinate_datatype)
            time_domain.value.expand(1)[-1] = time_box
            count = int(10 * random.random())
            time_domain.value.count.expand(1)[-1] = count

            # Iterate over the time cells within each time box
            for c in range(count):

                # Store IDs of objects in the active set
                object_index = active_object_id.nr_ids
                active_set_index.expand(1)[-1] = object_index
                nr_objects = int(random.random() * max_nr_objects)

                object_id = numpy.empty(nr_objects, dtype=ldm.dtype.ID)
                ldm.test.select_random_ids(object_id, max_nr_objects);
                active_object_id.expand(nr_objects)[object_index:] = object_id

                # Store property values of active objects
                discharge_values = \
                    numpy.arange(nr_objects, dtype=discharge_datatype)
                discharge.value.expand(nr_objects)[object_index:] = \
                    discharge_values

        ldm.assert_is_valid(dataset)

        del dataset

        dataset = ldm.open_dataset("outlets2.lue")
        phenomenon = dataset.phenomena["areas"]
        outlet_points = phenomenon.property_sets["outlets"]
        time_domain = outlet_points.time_domain
        clock = time_domain.clock

        self.assertEqual(clock.epoch.kind, ldm.Epoch.Kind.common_era)
        self.assertEqual(clock.epoch.origin, "2019-01-01")
        self.assertEqual(clock.epoch.calendar, ldm.Calendar.gregorian)
        self.assertEqual(clock.unit, ldm.Unit.day)
        self.assertEqual(clock.nr_units, 1)
