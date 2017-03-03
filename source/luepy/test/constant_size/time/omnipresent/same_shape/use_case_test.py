from functools import reduce
import os.path
import numpy
import lue
import lue_test


omnipresent = lue.constant_size.time.omnipresent


class UseCaseTest(lue_test.TestCase):


    def verify_property_set_2(self,
            property_set,
            ids):

        self.assertEqual(type(property_set), omnipresent.PropertySet)
        self.assertEqual(property_set.ids.dtype, numpy.uint64)
        self.assertEqual(len(property_set.ids.shape), 1)
        self.assertEqual(property_set.ids.shape[0], len(ids))
        self.assertArraysEqual(property_set.ids[:], ids)


    def verify_property_set_test_2(self,
            property_set,
            name,
            ids,
            boxes):

        self.assertEqual(property_set.name, name)
        self.assertEqual(type(property_set), omnipresent.PropertySet)
        self.assertEqual(property_set.ids.dtype, numpy.uint64)
        self.assertEqual(len(property_set.ids.shape), 1)
        self.assertEqual(property_set.ids.shape[0], len(ids))
        self.assertArraysEqual(property_set.ids[:], ids)

        self.assertArraysEqual(property_set.domain.space.boxes[:], boxes)



    def verify_property_1(self,
            property,
            nr_items,
            dtype,
            values):

        self.assertEqual(type(property), omnipresent.same_shape.Property)
        self.assertEqual(property.values.dtype, dtype)
        self.assertEqual(len(property.values.shape), 3)
        self.assertEqual(property.values.shape[0], nr_items)
        self.assertEqual(property.values.shape[1], 4)
        self.assertEqual(property.values.shape[2], 5)
        self.assertArraysEqual(property.values[:], values)


    def test_1(self):
        """
        - No space domain
        - Property with 2D values of the same shape
        """

        # Create and write a dataset. ------------------------------------------
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

        self.verify_property_set_2(property_set, ids_)


        # Now, add a property, whose values all have the same shape.
        value_shape = (4, 5)
        chunk_shape = (4, 5)
        value_type = numpy.int32
        property = omnipresent.same_shape.create_property(
            property_set, property_name, value_type, value_shape, chunk_shape)

        self.assertEqual(property.name, property_name)

        values = property.reserve(nr_items)
        values_ = numpy.arange(nr_items * reduce(lambda x, y: x * y,
            value_shape), dtype=value_type).reshape((nr_items,) + value_shape)
        values[:] = values_

        self.verify_property_1(property, nr_items, value_type, values_)


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
        self.verify_property_set_2(property_set, ids_)

        property = property_set[property_name]

        self.assertEqual(property.name, property_name)
        self.verify_property_1(property, nr_items, value_type, values_)


    def test_2(self):
        """
        - Space domain with extents
        - No discretization
        - Property with 1D values of different shape
        """

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "my_dataset_test_2.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"
        property_name = "my_property"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = omnipresent.create_property_set(phenomenon,
            property_set_name)

        nr_items = 500

        ids = property_set.reserve(nr_items)
        ids_ = numpy.array([id for id in range(nr_items)], numpy.uint64)
        ids[:] = ids_

        coordinate_dtype = numpy.float32
        rank = 2
        space_domain = omnipresent.create_space_box_domain(property_set,
            coordinate_dtype, rank)

        boxes = space_domain.reserve(nr_items)
        nr_coordinates_per_box = boxes.shape[1]
        boxes_ = numpy.arange(nr_items * nr_coordinates_per_box,
            dtype=coordinate_dtype).reshape(nr_items, nr_coordinates_per_box)
        boxes[:] = boxes_

        self.verify_property_set_test_2(property_set, property_set_name,
            ids_, boxes_)


        # # Now, add a property, whose values all have the same shape.
        # value_shape = (4, 5)
        # chunk_shape = (4, 5)
        # value_type = numpy.int32
        # property = omnipresent.same_shape.create_property(
        #     property_set, property_name, value_type, value_shape, chunk_shape)

        # self.assertEqual(property.name, property_name)

        # values = property.reserve(nr_items)
        # values_ = numpy.arange(nr_items * reduce(lambda x, y: x * y,
        #     value_shape), dtype=value_type).reshape((nr_items,) + value_shape)
        # values[:] = values_

        # self.verify_property_1(property, nr_items, value_type, values_)


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

        self.verify_property_set_test_2(property_set, property_set_name,
            ids_, boxes_)

        # property = property_set[property_name]

        # self.assertEqual(property.name, property_name)
        # self.verify_property_1(property, nr_items, value_type, values_)
