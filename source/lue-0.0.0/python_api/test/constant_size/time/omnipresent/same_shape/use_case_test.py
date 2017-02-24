from functools import reduce
import os.path
import numpy
import lue
import lue_test


# size of item collection: constant
# time domain type       : omnipresent
# shape per item         : same


class UseCaseTest(lue_test.TestCase):


    def verify_property_set_1(self,
            property_set):

        self.assertEqual(type(property_set),
            lue.constant_size.time.omnipresent.PropertySet)

        self.assertEqual(
            property_set.configuration.size_of_item_collection_type,
            lue.size_of_item_collection.constant)

        self.assertEqual(property_set.domain.configuration.time.type,
            lue.time_domain.omnipresent)
        self.assertEqual(property_set.domain.configuration.time.item_type,
            lue.time_domain_item.none)
        self.assertEqual(property_set.domain.configuration.space.type,
            lue.space_domain.omnipresent)
        self.assertEqual(property_set.domain.configuration.space.item_type,
            lue.space_domain_item.none)
        self.assertEqual(property_set.domain.configuration.space.mobility,
            lue.mobility.stationary)

        self.assertEqual(property_set.ids.dtype, numpy.uint64)
        self.assertEqual(len(property_set.ids.shape), 1)


    def verify_property_1(self,
            property):

        self.assertEqual(type(property),
            lue.constant_size.time.omnipresent.same_shape.Property)

        self.assertEqual(property.values.shape_per_item_type,
            lue.shape_per_item.same)

        self.assertEqual(property.values.dtype, numpy.int32)
        self.assertEqual(len(property.values.shape), 3)
        self.assertEqual(property.values.shape[0], 0)
        self.assertEqual(property.values.shape[1], 4)
        self.assertEqual(property.values.shape[2], 5)


    def test_1(self):

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "my_dataset.lue")
        universe_name = "my_universe"
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"
        property_name = "my_property"

        dataset = self.create_dataset(dataset_name)
        universe = dataset.add_universe(universe_name)
        phenomenon = universe.add_phenomenon(phenomenon_name)


        # Add a new property set. This returns a type with a special API,
        # which is dependent on the property set and time domain
        # configuration.
        property_set = phenomenon.add_property_set(property_set_name)

        self.verify_property_set_1(property_set)
        self.assertEqual(property_set.ids.shape[0], 0)

        # Add items. This is independent of whether or not there are
        # properties added to the set.
        nr_items = 500
        ids = property_set.reserve_items(nr_items)

        self.assertEqual(ids.shape[0], nr_items)

        ids_ = numpy.array([id for id in range(nr_items)], numpy.uint64)
        ids[:] = ids_

        self.assertArraysEqual(ids[:], ids_)


        # Now, add a property, whose values all have the same shape.
        value_shape = (4, 5)
        chunk_shape = (4, 5)
        value_type = numpy.int32
        property = property_set.add_property(property_name, value_type,
            value_shape, chunk_shape)
        self.assertEqual(property.name, property_name)
        self.verify_property_1(property)

        values = property.reserve_items(nr_items)
        self.assertEqual(values.shape[0], nr_items)

        values_ = numpy.arange(nr_items * reduce(lambda x, y: x * y,
            value_shape), dtype=value_type).reshape((nr_items,) + value_shape)

        values[:] = values_

        self.assertArraysEqual(values[:], values_)


        # Open and read the dataset. -------------------------------------------
        dataset = lue.open_dataset(dataset_name, "r")

        phenomena = dataset.phenomena
        self.assertEqual(len(phenomena), 0)

        universes = dataset.universes
        self.assertEqual(len(universes), 1)
        self.assertTrue(universe_name in universes)

        universe = universes[universe_name]
        phenomena = universe.phenomena
        self.assertEqual(len(phenomena), 1)
        self.assertTrue(phenomenon_name in phenomena)

        phenomenon = phenomena[phenomenon_name]
        property_sets = phenomenon.property_sets
        self.assertEqual(len(property_sets), 1)
        self.assertTrue(property_set_name in property_sets)

        property_set = property_sets[property_set_name]

        self.verify_property_set_1(property_set)
        self.assertEqual(property_set.ids.shape[0], nr_items)

        ids = property_set.ids

        self.assertEqual(ids.shape[0], nr_items)
        self.assertArraysEqual(ids[:], ids_)

        print(property_set)
        properties = property_set.properties
        # print(properties)
        # print(type(properties))
        # print(dir(properties))
        ### print(properties)
        self.assertEqual(len(properties), 1)
        self.assertTrue(property_name in properties)

        property = properties[property_name]
        ### print(property)

        self.verify_property_1(property)
        self.assertEqual(property.name, property_name)
