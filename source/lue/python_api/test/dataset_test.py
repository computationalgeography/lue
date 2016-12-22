import os
import lue
import lue_test


class DatasetTest(lue_test.TestCase):


    def test_create(self):

        dataset_name = "dataset_create.lue"

        lue_test.remove_file_if_existant(dataset_name)

        dataset = lue.create_dataset(dataset_name)

        self.assertTrue(os.path.isfile(dataset_name))


    def test_pathname(self):

        dataset_name = "dataset_pathname.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertEqual(dataset.pathname, dataset_name)


    def test_open(self):

        dataset_name = "dataset_open.lue"
        dataset = self.create_dataset(dataset_name)

        existing_dataset = lue.open_dataset(dataset_name, lue.access_flag.ro)


    def test_add_universe(self):

        dataset_name = "dataset_add_universe.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertTrue("my_universe" not in dataset.universes)
        dataset.add_universe("my_universe")
        self.assertTrue("my_universe" in dataset.universes)


    def test_add_universe_twice(self):

        dataset_name = "dataset_add_universe_twice.lue"
        dataset = self.create_dataset(dataset_name)

        dataset.add_universe("my_universe")
        self.assertRaises(RuntimeError, dataset.add_universe,
            "my_universe")


    def test_add_phenomenon(self):

        dataset_name = "dataset_add_phenomenon.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertTrue("my_phenomenon" not in dataset.phenomena)
        dataset.add_phenomenon("my_phenomenon")
        self.assertTrue("my_phenomenon" in dataset.phenomena)


    def test_add_phenomenon_twice(self):

        dataset_name = "dataset_add_phenomenon_twice.lue"
        dataset = self.create_dataset(dataset_name)

        dataset.add_phenomenon("my_phenomenon")
        self.assertRaises(RuntimeError, dataset.add_phenomenon,
            "my_phenomenon")
