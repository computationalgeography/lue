import os
import lue
import lue_test


class DatasetTest(lue_test.TestCase):


    def test_create(self):

        dataset_name = "dataset_create.lue"

        lue_test.remove_file_if_existant(dataset_name)

        dataset = lue.create_dataset(dataset_name)

        self.assertDatasetIsValid(dataset)
        self.assertTrue(os.path.isfile(dataset_name))
        self.assertTrue(os.access(dataset_name, os.F_OK))
        self.assertTrue(os.access(dataset_name, os.R_OK))
        self.assertTrue(os.access(dataset_name, os.W_OK))
        self.assertFalse(os.access(dataset_name, os.X_OK))


    def test_object_name(self):

        dataset_name = "dataset_object_name.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertDatasetIsValid(dataset)
        self.assertEqual(dataset.id.name, "")


    def test_object_pathname(self):

        dataset_name = "dataset_object_pathname.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertDatasetIsValid(dataset)
        self.assertEqual(dataset.id.pathname, "/")


    def test_pathname(self):

        dataset_name = "dataset_pathname.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertDatasetIsValid(dataset)
        self.assertEqual(dataset.pathname, dataset_name)


    def test_open_existing_readable(self):

        dataset_name = "dataset_open_existing_readable.lue"
        dataset = self.create_dataset(dataset_name)
        existing_dataset = lue.open_dataset(dataset_name, "r")


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


    def test_query_version(self):
        dataset_name = "dataset_query_version.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertEqual(dataset.lue_version, lue.__version__)
        self.assertEqual(dataset.lue_version, lue.lue_version)

        self.assertEqual(dataset.hdf5_version, lue.hdf5.__version__)
        self.assertEqual(dataset.hdf5_version, lue.hdf5.hdf5_version)


    # TODO gh178
    # def test_add_log_message(self):
    #     dataset_name = "dataset_add_log_message.lue"
    #     dataset = self.create_dataset(dataset_name)

    #     self.assertEqual(dataset.log, [])

