import os.path
import numpy
import lue
import lue_test


omnipresent = lue.constant_size.time.omnipresent
shared = lue.constant_size.time.located.shared


class DiscretizedTimeTest(lue_test.TestCase):

    def verify_time_domain_with_boxes(self,
            property_set,
            name,
            boxes):

        self.assertEqual(property_set.name, name)
        self.assertEqual(type(property_set), shared.PropertySet)
        self.assertArraysEqual(property_set.domain.time.items[:], boxes)


    def verify_property_values(self,
            property,
            name,
            nr_domain_items,
            nr_items,
            value_shape,
            dtype,
            values):

        self.assertEqual(property.name, name)
        self.assertEqual(
            type(property), shared.constant_shape.same_shape.Property)
        self.assertEqual(len(property.values.shape), 2 + len(value_shape))
        self.assertEqual(property.values.shape[0], nr_domain_items)
        self.assertEqual(property.values.shape[1], nr_items)
        self.assertEqual(tuple(property.values.shape[2:]), value_shape)
        self.assertEqual(property.values.dtype, dtype)

        for d in range(nr_domain_items):
            for i in range(nr_items):
                self.assertArraysEqual(property.values[d][i], values[d][i])


        self.assertTrue(property.time_is_discretized)
        self.assertTrue(not property.space_is_discretized)

        discretization_property = property.time_discretization
        self.assertEqual(
            type(discretization_property),
            omnipresent.same_shape.Property)


    def verify_discretization_property_values(self,
            property,
            name,
            nr_items,
            dtype,
            values):

        self.assertEqual(property.name, name)
        self.assertEqual(type(property), omnipresent.same_shape.Property)
        self.assertEqual(property.values.dtype, dtype)
        self.assertEqual(len(property.values.shape), 1)
        self.assertEqual(property.values.shape[0], nr_items)
        self.assertArraysEqual(property.values[:], values)


    def test_timeseries(self):
        """
        - Time domain with extents
        - Discretization property
        - Linked value property
        """

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(
            os.path.dirname(__file__), "test_timeseries.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = shared.create_property_set(
            phenomenon, property_set_name)
        nr_items = 100

        property_set.reserve(nr_items)[:] = numpy.arange(nr_items)


        # Time domain
        time_domain = shared.create_time_box_domain(
            property_set, lue.Clock(lue.unit.day, 1))

        # A box is defined by a begin and end time point (two coordinates
        # per box)
        nr_boxes = 4
        boxes = time_domain.reserve(nr_boxes)
        nr_coordinates_per_box = boxes.shape[1]
        self.assertEqual(nr_coordinates_per_box, 2)
        boxes_ = numpy.array([
            # Time boxes of duration 10 days
                [10,  20],
                [30,  40],
                [50,  60],
                [70,  80],
            ], boxes.dtype)
        boxes[:] = boxes_

        self.verify_time_domain_with_boxes(property_set,
            property_set_name, boxes_)


        # Discretization property
        # - Since the time domain is shared for all items, discretization
        #   information must also be shared for all domain items. This
        #   means that discretization information is a property of
        #   a different phenomenon, which can be linked to the discretized
        #   property.
        # - Here we assume that the shape of the values doesn't change
        #   through time. That means that the discretization also doesn't
        #   change through time. That means that, in order to define
        #   the discretization, we only need to store a single value for
        #   'nr_steps'.

        # Given the 10 day time boxes, configure time steps to be 2
        # days long
        nr_steps = 5
        discretization_phenomenon_name = "time_boxes"
        discretization_phenomenon = dataset.add_phenomenon(
            discretization_phenomenon_name)
        discretization_property_set_name = "discretization"
        discretization_property_set = omnipresent.create_property_set(
            discretization_phenomenon, discretization_property_set_name)
        discretization_property_set.reserve(1)[0] = 12345

        discretization_property_name = "nr_steps"
        discretization_value_type = numpy.uint32
        discretization_property = omnipresent.same_shape.create_property(
            discretization_property_set, discretization_property_name,
            discretization_value_type)

        nr_steps_ = numpy.array([nr_steps], discretization_value_type)
        discretization_property.reserve(1)[:] = nr_steps

        self.verify_discretization_property_values(
            discretization_property, discretization_property_name,
            1, discretization_value_type, nr_steps_)


        # Value property
        # - Per time box, per item, an nD value which is
        #   discretized according to nr_steps in discretization property
        # - The HDF5 dataset is shaped as folows:
        #     [nr_time_domain_items, nr_items, value_rank]
        value_property_name = "my_value_property"
        value_type = numpy.float64
        value_shape = (nr_steps,)
        property = shared.constant_shape.same_shape.create_property(
            property_set, value_property_name, value_type, value_shape)
        property.discretize_time(discretization_property)

        values = property.reserve(nr_boxes, nr_items)
        values_ = numpy.array(
            [x * numpy.random.rand() for x in range(
                nr_boxes * nr_items * nr_steps)],
            dtype=value_type).reshape(values.shape)
        values[:] = values_

        self.verify_property_values(property, value_property_name,
            nr_boxes, nr_items, value_shape, value_type, values_)

        self.assertDatasetIsValid(dataset)


        # Open and read the dataset. -------------------------------------------
        dataset = lue.open_dataset(dataset_name, "r")

        universes = dataset.universes
        self.assertEqual(len(universes), 0)

        phenomena = dataset.phenomena
        self.assertEqual(len(phenomena), 2)
        self.assertTrue(phenomenon_name in phenomena)
        self.assertTrue(discretization_phenomenon_name in phenomena)

        phenomenon = phenomena[phenomenon_name]
        property_sets = phenomenon.property_sets
        self.assertEqual(len(property_sets), 1)
        self.assertTrue(property_set_name in property_sets)

        property_set = property_sets[property_set_name]

        self.verify_time_domain_with_boxes(
            property_set, property_set_name, boxes_)


        discretization_phenomenon = phenomena[discretization_phenomenon_name]
        discretization_property_sets = discretization_phenomenon.property_sets
        self.assertEqual(len(discretization_property_sets), 1)
        self.assertTrue(
            discretization_property_set_name in discretization_property_sets)

        discretization_property = \
            discretization_property_set[discretization_property_name]

        self.verify_discretization_property_values(
            discretization_property, discretization_property_name,
            1, discretization_value_type, nr_steps_)


        value_property = property_set[value_property_name]

        self.verify_property_values(
            value_property, value_property_name,
            nr_boxes, nr_items, value_shape, value_type, values_)
