import numpy
import lue
import lue_test


class UseCaseTest(lue_test.TestCase):


    def test_use_case_1(self):

        dataset_name = "o_vs_use_case_1.lue"
        dataset = self.create_dataset(dataset_name)
        universe = dataset.add_universe("universe")
        phenomenon = universe.add_phenomenon("phenomenon")


        # Add a new property set. This returns a type with a special API,
        # which is dependent on the item/time domain configuration.
        space_configuration = lue.SpaceDomainConfiguration(
            lue.space_domain.located, lue.space_domain_item.box)
        property_set = phenomenon.add_property_set("property_set",
            space_configuration)

        self.assertEqual(property_set.domain.configuration.time.type,
            lue.time_domain.omnipresent)
        self.assertEqual(property_set.domain.configuration.time.item_type,
            lue.time_domain_item.none)
        self.assertEqual(property_set.domain.configuration.space.type,
            lue.space_domain.located)
        self.assertEqual(property_set.domain.configuration.space.item_type,
            lue.space_domain_item.box)
        self.assertEqual(property_set.ids.dtype, numpy.uint64)
        self.assertEqual(len(property_set.ids.shape), 1)
        self.assertEqual(property_set.ids.shape[0], 0)

        rank = 2

        space_domain = property_set.domain.space_domain
        self.assertEqual(space_domain.boxes.dtype, numpy.float64)
        self.assertEqual(len(space_domain.boxes.shape), 2)
        self.assertEqual(space_domain.boxes.shape[0], 0)
        self.assertEqual(space_domain.boxes.shape[1], 2 * rank)


        # Add items. This is independent of whether or there are properties
        # added to the set.
        nr_items = 500
        items = property_set.reserve_items(nr_items)
        space_domain.boxes.reserve_items(nr_items)

        self.assertEqual(items.shape[0], nr_items)
        self.assertEqual(space_domain.boxes.shape[0], nr_items)

        items_ = numpy.array([id for id in xrange(nr_items)], numpy.uint64)
        items[:] = items_

        self.assertArraysEqual(items[:], items_)

        nr_coordinates_per_box = space_domain.boxes.shape[1]
        box_shape = (nr_coordinates_per_box,)
        boxes = numpy.arange(nr_items * nr_coordinates_per_box,
            dtype=numpy.float64).reshape(nr_items, nr_coordinates_per_box)
        space_domain.boxes[:] = boxes

        self.assertArraysEqual(space_domain.boxes[:], boxes)


        # Now, add a property, whose values all have different shapes.
        value_type = numpy.int32

        property = property_set.add_property("property", value_type, rank)

        self.assertEqual(property.name, "property")
        self.assertEqual(property.values.dtype, numpy.int32)
        self.assertEqual(property.values.rank, 2)
        self.assertEqual(len(property.values), 0)

        value_shapes = (10 * (numpy.random.rand(nr_items, rank) + 1)).astype(
            numpy.uint64)

        values = property.reserve_items(value_shapes)

        self.assertEqual(len(values), nr_items)

        for i in xrange(nr_items):
            for r in xrange(rank):
                self.assertEqual(property.values[i].shape[r],
                    value_shapes[i][r])


        for i in xrange(nr_items):
            shape = property.values[i].shape
            values_ = (10 * numpy.random.rand(*shape)).astype(value_type)
            values[i][:] = values_
            self.assertArraysEqual(values[i][:], values_)
