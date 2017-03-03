import os.path
import numpy
import lue
import lue_test


omnipresent = lue.constant_size.time.omnipresent


class UseCaseTest(lue_test.TestCase):


    def verify_property_set_1(self,
            property_set,
            ids):

        self.assertEqual(type(property_set), omnipresent.PropertySet)
        self.assertEqual(property_set.ids.dtype, numpy.uint64)
        self.assertEqual(len(property_set.ids.shape), 1)
        self.assertEqual(property_set.ids.shape[0], 500)
        self.assertArraysEqual(property_set.ids[:], ids)


    def verify_property_1(self,
            property,
            nr_items,
            rank,
            shapes,
            dtype,
            values):

        self.assertEqual(type(property), omnipresent.different_shape.Property)
        self.assertEqual(property.values.rank, rank)
        self.assertEqual(len(property.values), nr_items)
        self.assertEqual(property.values.dtype, numpy.int32)

        for i in range(nr_items):

            for r in range(rank):
                self.assertEqual(property.values[i].shape[r], shapes[i][r])

            self.assertArraysEqual(values[i][:], values[i])


    def test_1(self):
        """
        - No space domain
        - Property with 2D values of different shape
        """
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "my_dataset_test_1.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"
        property_name = "my_property"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = omnipresent.create_property_set(phenomenon,
            property_set_name)

        self.assertEqual(property_set.name, property_set_name)

        nr_items = 500
        ids = property_set.reserve(nr_items)
        ids_ = numpy.array([id for id in range(nr_items)], numpy.uint64)
        ids[:] = ids_

        self.assertEqual(property_set.name, property_set_name)
        self.verify_property_set_1(property_set, ids_)

        # Now, add a property, whose values all have different shapes.
        rank = 2
        value_type = numpy.int32
        property = omnipresent.different_shape.create_property(
            property_set, property_name, value_type, rank)

        self.assertEqual(property.name, property_name)

        shapes = (10 * (numpy.random.rand(nr_items, rank) + 1)).astype(
            numpy.uint64)
        values = property.reserve(shapes)

        values_ = []

        for i in range(nr_items):
            shape = property.values[i].shape
            values_.append((10 * numpy.random.rand(*shape)).astype(value_type))
            values[i][:] = values_[i]

        self.verify_property_1(property, nr_items, rank, shapes, value_type,
            values_)


        # Open and read the dataset. -------------------------------------------
        dataset = lue.open_dataset(dataset_name, "r")

        universes = dataset.universes
        self.assertEqual(len(universes), 0)

        phenomena = dataset.phenomena
        self.assertEqual(len(phenomena), 1)
        self.assertTrue(phenomenon_name in phenomena)

        phenomenon = phenomena[phenomenon_name]
        property_sets = phenomenon.property_sets
        self.assertEqual(len(property_sets), 1)
        self.assertTrue(property_set_name in property_sets)

        property_set = property_sets[property_set_name]

        self.assertEqual(property_set.name, property_set_name)
        self.verify_property_set_1(property_set, ids_)

        property = property_set[property_name]

        self.assertEqual(property.name, property_name)
        self.verify_property_1(property, nr_items, rank, shapes, value_type,
            values_)
