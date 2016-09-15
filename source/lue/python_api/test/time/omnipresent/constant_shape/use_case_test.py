import numpy
import lue
import lue_test


class UseCaseTest(lue_test.TestCase):


    def test_use_case_1(self):

        dataset_name = "o_cs_use_case_1.lue"
        dataset = self.create_dataset(dataset_name)
        universe = dataset.add_universe("universe")
        phenomenon = universe.add_phenomenon("phenomenon")


        # Add a new property set. This returns a type with a special API,
        # which is dependent on the item/time domain configuration.
        property_set = phenomenon.add_property_set("property_set")

        self.assertEqual(property_set.domain.configuration.time.type,
            lue.time_domain.omnipresent)
        self.assertEqual(property_set.domain.configuration.space.type,
            lue.space_domain.omnipresent)
        self.assertEqual(property_set.items.dtype, numpy.uint64)
        ### self.assertEqual(len(property_set.items.shape), 1)
        ### self.assertEqual(property_set.items.shape[0], 0)


        ### # Add items. This is independent of whether or there are properties
        ### # added to the set.
        ### nr_items = 500
        ### items = property_set.reserve_items(nr_items)

        ### self.assertEqual(items.shape[0], nr_items)

        ### items_ = numpy.array([id for id in xrange(nr_items)], numpy.uint64)
        ### items[:] = items_

        ### self.assertArraysEqual(items[:], items_)


        ### # Now, add a property, whose values all have the same shape.
        ### value_shape = (4, 5)
        ### chunk_shape = (4, 5)
        ### value_type = numpy.int32
        ### property = property_set.add_property("property", value_type,
        ###     value_shape, chunk_shape)
        ### self.assertEqual(property.name, "property")
        ### self.assertEqual(property.values.dtype, numpy.int32)
        ### self.assertEqual(len(property.values.shape), 3)
        ### self.assertEqual(property.values.shape[0], 0)
        ### self.assertEqual(property.values.shape[1], 4)
        ### self.assertEqual(property.values.shape[2], 5)

        ### values = property.reserve_items(nr_items)

        ### self.assertEqual(values.shape[0], nr_items)

        ### values_ = numpy.arange(nr_items * reduce(lambda x, y: x * y, value_shape),
        ###     dtype=numpy.int32).reshape((nr_items,) + value_shape)

        ### values[:] = values_

        ### self.assertArraysEqual(values[:], values_)
