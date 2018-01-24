# from functools import reduce
import os.path
import numpy
import lue
import lue_test


omnipresent = lue.constant_collection.time.omnipresent


class ScalarTest(lue_test.TestCase):

    def verify_property_values(self,
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


    def test_scalar(self):

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "test_scalar.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"
        property_name = "my_property"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = omnipresent.create_property_set(phenomenon,
            property_set_name)
        nr_items = 500

        ids = property_set.ids.reserve(nr_items)
        ids[:] = numpy.arange(nr_items)

        value_type = numpy.int32
        property = omnipresent.same_shape.create_property(
            property_set, property_name, value_type)

        values = property.values.reserve(nr_items)
        values_ = numpy.arange(nr_items, dtype=value_type)
            # .reshape((nr_items, 1))
        values[:] = values_

        self.verify_property_values(property, property_name,
            nr_items, value_type, values_)

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
            nr_items, value_type, values_)





    # def test_values(self):
    #     """
    #     - No space domain
    #     - Property with 2D values of the same shape
    #     """

    #     # Create and write a dataset. ------------------------------------------
    #     dataset_name = self.relative_pathname(os.path.dirname(__file__),
    #         "test_values.lue")
    #     phenomenon_name = "my_phenomenon"
    #     property_set_name = "my_property_set"
    #     property_name = "my_property"

    #     dataset = self.create_dataset(dataset_name)
    #     phenomenon = dataset.add_phenomenon(phenomenon_name)
    #     property_set = omnipresent.create_property_set(phenomenon,
    #         property_set_name)
    #     nr_items = 500


    #     # Now, add a property, whose values all have the same shape.
    #     value_shape = (4, 5)
    #     chunk_shape = (4, 5)
    #     value_type = numpy.int32
    #     property = omnipresent.same_shape.create_property(
    #         property_set, property_name, value_type, value_shape, chunk_shape)

    #     values = property.values.reserve(nr_items)
    #     values_ = numpy.arange(nr_items * reduce(lambda x, y: x * y,
    #         value_shape), dtype=value_type).reshape((nr_items,) + value_shape)
    #     values[:] = values_

    #     self.verify_property_values(property, property_name,
    #         nr_items, value_type, values_)


    #     # Open and read the dataset. -------------------------------------------
    #     dataset = lue.open_dataset(dataset_name, "r")

    #     universes = dataset.universes
    #     self.assertEqual(len(universes), 0)

    #     phenomena = dataset.phenomena
    #     self.assertEqual(len(phenomena), 1)
    #     self.assertTrue(phenomenon_name in phenomena)

    #     phenomenon = phenomena[phenomenon_name]
    #     property_sets = phenomenon.property_sets
    #     self.assertEqual(len(property_sets), 1)
    #     self.assertTrue(property_set_name in property_sets)

    #     property_set = property_sets[property_set_name]
    #     property = property_set[property_name]

    #     self.verify_property_values(property, property_name,
    #         nr_items, value_type, values_)
