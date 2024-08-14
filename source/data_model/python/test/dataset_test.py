import os

import numpy as np

import lue
import lue.data_model as ldm
import lue_test


class DatasetTest(lue_test.TestCase):
    def test_create(self):
        dataset_name = "dataset_create.lue"

        lue_test.remove_file_if_existent(dataset_name)

        dataset = ldm.create_dataset(dataset_name)

        self.assertDatasetIsValid(dataset)
        self.assertTrue(os.path.isfile(dataset_name))
        self.assertTrue(os.access(dataset_name, os.F_OK))
        self.assertTrue(os.access(dataset_name, os.R_OK))
        self.assertTrue(os.access(dataset_name, os.W_OK))

        if os.name != "nt":
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
        existing_dataset = ldm.open_dataset(dataset_name, "r")

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
        self.assertRaises(RuntimeError, dataset.add_universe, "my_universe")

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
        self.assertRaises(RuntimeError, dataset.add_phenomenon, "my_phenomenon")

    def test_query_version(self):
        dataset_name = "dataset_query_version.lue"
        dataset = self.create_dataset(dataset_name)

        self.assertEqual(dataset.lue_version, lue.__version__)
        self.assertEqual(dataset.lue_version, lue.lue_version)

        self.assertEqual(dataset.hdf5_version, ldm.hdf5.__version__)
        self.assertEqual(dataset.hdf5_version, ldm.hdf5.hdf5_version)

    def test_release_dataset(self):
        # gh431: A Python variable referencing a time or space domain's value instance kept
        # the HDF5 file open, even if all related variables had gone out of scope. This was
        # due to an error in our understanding of pybind11's return value policies. We created
        # a dangling pointer.
        # This issue surfaced only on Windows.

        dataset_name = "dataset_release_dataset.lue"
        lue_test.remove_file_if_existent(dataset_name)

        def create_dataset():
            dataset = self.create_dataset(dataset_name)

        def update_dataset():
            dataset = ldm.open_dataset(dataset_name, "w")
            my_phenomenon = dataset.add_phenomenon("my_phenomenon")

            # Add property set with a space domain
            space_configuration = ldm.SpaceConfiguration(
                ldm.Mobility.stationary, ldm.SpaceDomainItemType.box
            )
            my_property_set1 = my_phenomenon.add_property_set(
                "my_property_set1", space_configuration, np.float32, 2
            )
            space_domain = my_property_set1.space_domain
            value1 = space_domain.value  # <--- Used to dangle

            # Add property set with a time domain
            epoch = ldm.Epoch(
                ldm.Epoch.Kind.common_era, "2000-01-02T12:34:00", ldm.Calendar.gregorian
            )
            clock = ldm.Clock(epoch, ldm.Unit.month, 4)
            time_configuration = ldm.TimeConfiguration(ldm.TimeDomainItemType.box)

            my_property_set2 = my_phenomenon.add_property_set(
                "my_property_set2", time_configuration, clock
            )
            time_domain = my_property_set2.time_domain
            value2 = time_domain.value  # <--- Used to dangle

            # All variables will now go out of scope. The result must be that nobody is preventing
            # the dataset from being used again.

        # Create dataset and do not refer to it anymore
        create_dataset()
        self.assertTrue(os.path.exists(dataset_name))

        # Update dataset and do not refer to it anymore
        update_dataset()

        # Since nobody is referring to the dataset anymore, we should be able to delete it.
        # Before the fix:
        # PermissionError: [WinError 32] The process cannot access the file because it is being
        # used by another process: 'dataset_release_dataset.lue'
        os.remove(dataset_name)
        self.assertFalse(os.path.exists(dataset_name))

    # TODO gh178
    # def test_add_log_message(self):
    #     dataset_name = "dataset_add_log_message.lue"
    #     dataset = self.create_dataset(dataset_name)

    #     self.assertEqual(dataset.log, [])

    def test_release_dataset2(self):
        # gh570: Similar problem as with gh431, tested above in test_release_dataset

        dataset_name = "dataset_release_dataset2.lue"
        lue_test.remove_file_if_existent(dataset_name)

        def create_dataset():
            dataset = self.create_dataset(dataset_name)

        def update_dataset():
            dataset = ldm.open_dataset(dataset_name, "w")
            my_phenomenon = dataset.add_phenomenon("my_phenomenon")

            clock = ldm.Clock(
                ldm.Epoch(
                    ldm.Epoch.Kind.common_era,
                    "2000-01-02T12:34:00",
                    ldm.Calendar.gregorian,
                ),
                ldm.Unit.month,
                4,
            )
            time_configuration = ldm.TimeConfiguration(ldm.TimeDomainItemType.box)

            space_configuration = ldm.SpaceConfiguration(
                ldm.Mobility.stationary, ldm.SpaceDomainItemType.point
            )

            my_property_set = my_phenomenon.add_property_set(
                "my_property_set",
                time_configuration,
                clock,
                space_configuration,
                np.dtype(np.float64),
                rank=2,
            )

            time_box = 1

            my_property_set.object_tracker.active_set_index.expand(time_box)[:] = 0.6
            my_property_set.time_domain.value.expand(time_box)[:] = np.array([0, 5])

            my_phenomenon.add_property_set(
                "space", space_configuration, np.dtype(np.float64), rank=2
            )
            my_property_set.add_property(
                "discretization", dtype=ldm.dtype.Count, shape=(2,)
            )

            my_property = my_property_set.add_property(
                "my_property",
                dtype=np.dtype(np.float32),
                shape=(100, 1),
                value_variability=ldm.ValueVariability.variable,
            )

            my_property.set_space_discretization(
                ldm.SpaceDiscretization.regular_grid, my_property_set.discretization
            )

            discretization_property = (
                my_property.space_discretization_property()
            )  # <--- Used to dangle

            # All variables will now go out of scope. The result must be that nobody is preventing
            # the dataset from being used again.

        # Create dataset and do not refer to it anymore
        create_dataset()
        self.assertTrue(os.path.exists(dataset_name))

        # Update dataset and do not refer to it anymore
        update_dataset()

        # Since nobody is referring to the dataset anymore, we should be able to delete it.
        # Before the fix:
        # PermissionError: [WinError 32] The process cannot access the file because it is being
        # used by another process: 'dataset_release_dataset2.lue'
        os.remove(dataset_name)
        self.assertFalse(os.path.exists(dataset_name))
