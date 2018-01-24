from functools import reduce
import os.path
import numpy
import lue
import lue_test


omnipresent = lue.constant_collection.time.omnipresent


class PropertySetTest(lue_test.TestCase):


    def verify_property_set_ids(self,
            property_set,
            ids):

        self.assertEqual(type(property_set), omnipresent.PropertySet)
        self.assertEqual(property_set.ids.dtype, numpy.uint64)
        self.assertEqual(len(property_set.ids.shape), 1)
        self.assertEqual(property_set.ids.shape[0], len(ids))
        self.assertArraysEqual(property_set.ids[:], ids)


    def test_ids(self):
        """
        - No space domain
        """

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "test_ids.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = omnipresent.create_property_set(phenomenon,
            property_set_name)

        self.assertEqual(property_set.name, property_set_name)

        nr_items = 500
        ids = property_set.ids.reserve(nr_items)
        ids_ = numpy.array([id for id in range(nr_items)], numpy.uint64)
        ids[:] = ids_

        self.verify_property_set_ids(property_set, ids_)

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

        self.assertEqual(property_set.name, property_set_name)
        self.verify_property_set_ids(property_set, ids_)


    def test_shared_ids(self):

        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "test_shared_ids.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name1 = "my_property_set1"
        property_set_name2 = "my_property_set2"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set1 = omnipresent.create_property_set(phenomenon,
            property_set_name1)
        property_set2 = omnipresent.create_property_set(phenomenon,
            property_set_name2, property_set1.ids)

        nr_items = 5

        ids = numpy.array([5, 4, 3, 2, 1], numpy.uint64)
        property_set1.ids.reserve(nr_items)[:] = ids
        self.assertArraysEqual(property_set2.ids[:], ids)

        ids = numpy.array([15, 14, 13, 12, 11], numpy.uint64)
        property_set2.ids[:] = ids
        self.assertArraysEqual(property_set1.ids[:], ids)


    def verify_space_domain_with_boxes(self,
            property_set,
            name,
            boxes):

        self.assertEqual(property_set.name, name)
        self.assertEqual(type(property_set), omnipresent.PropertySet)
        self.assertArraysEqual(property_set.domain.space.items[:], boxes)


    def test_space_boxes(self):
        """
        - Space domain with extents
        - No discretization
        - No property
        """

        # Create and write a dataset. ------------------------------------------
        dataset_name = self.relative_pathname(os.path.dirname(__file__),
            "my_dataset_test_2.lue")
        phenomenon_name = "my_phenomenon"
        property_set_name = "my_property_set"

        dataset = self.create_dataset(dataset_name)
        phenomenon = dataset.add_phenomenon(phenomenon_name)
        property_set = omnipresent.create_property_set(phenomenon,
            property_set_name)
        nr_items = 500

        coordinate_dtype = numpy.float32
        rank = 2
        space_domain = omnipresent.create_space_box_domain(property_set,
            coordinate_dtype, rank)

        boxes = space_domain.reserve(nr_items)
        nr_coordinates_per_box = boxes.shape[1]
        boxes_ = numpy.arange(nr_items * nr_coordinates_per_box,
            dtype=coordinate_dtype).reshape(nr_items, nr_coordinates_per_box)
        boxes[:] = boxes_

        self.verify_space_domain_with_boxes(property_set,
            property_set_name, boxes_)

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

        self.verify_space_domain_with_boxes(property_set,
            property_set_name, boxes_)
