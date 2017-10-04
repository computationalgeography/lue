import os.path
import numpy
import lue
import lue_test


omnipresent = lue.constant_size.time.omnipresent


class UseCaseTest(lue_test.TestCase):


    def verify_property_values(self,
            property,
            name,
            nr_items,
            rank,
            shapes,
            dtype,
            values):

        self.assertEqual(property.name, name)
        self.assertEqual(type(property), omnipresent.different_shape.Property)
        self.assertEqual(property.values.rank, rank)
        self.assertEqual(len(property.values), nr_items)
        self.assertEqual(property.values.dtype, dtype)

        for i in range(nr_items):

            for r in range(rank):
                self.assertEqual(property.values[i].shape[r], shapes[i][r])

            self.assertArraysEqual(values[i][:], values[i])


    def test_values(self):
        """
        - No space domain
        - Property with 2D values of different shape
        """

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "test_values.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"
        property_name = "my_property"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = omnipresent.create_property_set(phenomenon,
            property_set_name)
        nr_items = 500

        property_set.reserve(nr_items)[:] = numpy.arange(nr_items)


        # Now, add a property, whose values all have different shapes.
        rank = 2
        value_type = numpy.int32
        property = omnipresent.different_shape.create_property(
            property_set, property_name, value_type, rank)

        shapes = (10 * (numpy.random.rand(nr_items, rank) + 1)).astype(
            numpy.uint64)
        values = property.reserve(shapes)

        values_ = []
        for i in range(nr_items):
            shape = property.values[i].shape
            values_.append((10 * numpy.random.rand(*shape)).astype(value_type))
            values[i][:] = values_[i]

        self.verify_property_values(property, property_name,
            nr_items, rank, shapes, value_type, values_)

        self.assertDatasetIsValid(dataset)


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
        property = property_set[property_name]

        self.verify_property_values(property, property_name,
            nr_items, rank, shapes, value_type, values_)
