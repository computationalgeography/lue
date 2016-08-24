import os
import unittest
import lue
import lue_test


class CollectionTest(unittest.TestCase):


    def test_in(self):
        dataset_name = "collection_in.lue"

        lue_test.remove_file_if_existant(dataset_name)

        dataset = lue.create_dataset(dataset_name)
        collection = dataset.phenomena

        self.assertTrue("my_item" not in collection)

        collection.add("my_item")
        self.assertTrue("my_item" in collection)


    def test_names(self):
        dataset_name = "collection_names.lue"

        lue_test.remove_file_if_existant(dataset_name)

        dataset = lue.create_dataset(dataset_name)
        collection = dataset.phenomena

        names = collection.names
        self.assertEqual(len(collection.names), 0)

        collection.add("my_item1")
        names = collection.names
        self.assertEqual(len(names), 1)
        self.assertEqual(names, ["my_item1"])

        collection.add("my_item2")
        names = collection.names
        self.assertEqual(len(names), 2)
        self.assertEqual(names, ["my_item1", "my_item2"])


    def test_item(self):
        dataset_name = "collection_item.lue"

        lue_test.remove_file_if_existant(dataset_name)

        dataset = lue.create_dataset(dataset_name)
        collection = dataset.phenomena

        collection.add("my_item")

        item = collection["my_item"]


    def test_non_existing_item(self):
        dataset_name = "collection_non_existing_item.lue"

        lue_test.remove_file_if_existant(dataset_name)

        dataset = lue.create_dataset(dataset_name)
        collection = dataset.phenomena

        self.assertRaises(RuntimeError, collection.__getitem__, "my_item")
