import numpy as np

import lue.data_model as ldm
import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class WriteArrayTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_write_constant_array(self):
        # Create an array, write it to a dataset, read it back in, and verify the array read is equal to the
        # array written.

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.int32
        # fill_value = 0

        # Create an array with random values and some no-data
        array_written = lfr.uniform(
            array_shape, dtype, 1, 10, partition_shape=partition_shape
        )
        array_written = lfr.where(array_written < 8, array_written)

        dataset_pathname = "write_constant_array.lue"
        phenomenon_name = "earth"
        property_set_name = "continent"
        layer_name = "elevation"
        space_box = [0, 0, 6000, 4000]
        array_pathname = "{}/{}/{}/{}".format(
            dataset_pathname, phenomenon_name, property_set_name, layer_name
        )

        dataset = ldm.create_dataset(dataset_pathname)
        raster_view = ldm.hl.create_raster_view(
            dataset, phenomenon_name, property_set_name, array_shape, space_box
        )
        raster_view.add_layer(layer_name, dtype)

        # Let go of the dataset
        del raster_view
        del dataset

        # Write and wait until finished
        lfr.write_array(array_written, array_pathname).get()

        array_read = lfr.read_array(array_pathname, partition_shape=partition_shape)

        self.assertTrue(lfr.all(array_read == array_written).future.get())

    @lue_test.framework_test_case
    def test_write_variable_array(self):
        # Create t arrays, write them to a dataset, ready them back in,
        # and verify the arrays read are equal to the arrays written.

        array_shape = (60, 40)
        partition_shape = (10, 10)
        dtype = np.int32
        fill_value = 0
        nr_time_steps = 7

        # Create t arrays with random values and some no-data
        arrays_written = []

        for t in range(nr_time_steps):
            array = lfr.uniform(
                array_shape, dtype, 1, 10, partition_shape=partition_shape
            )
            array = lfr.where(array < 8, array)
            arrays_written.append(array)

        dataset_pathname = "write_variable_array.lue"
        phenomenon_name = "earth"
        property_set_name = "continent"
        layer_name = "elevation"
        space_box = [0, 0, 6000, 4000]
        array_pathname = "{}/{}/{}/{}".format(
            dataset_pathname, phenomenon_name, property_set_name, layer_name
        )

        epoch = ldm.Epoch(
            ldm.Epoch.Kind.common_era, "2021-04-01", ldm.Calendar.gregorian
        )
        clock = ldm.Clock(epoch, ldm.Unit.day, 1)
        time_box = [0, 8]

        dataset = ldm.create_dataset(dataset_pathname)
        raster_view = ldm.hl.create_raster_view(
            dataset,
            phenomenon_name,
            property_set_name,
            clock,
            nr_time_steps,
            time_box,
            array_shape,
            space_box,
        )
        raster_view.add_layer(layer_name, dtype)

        # Let go of the dataset
        del raster_view
        del dataset

        for t in range(nr_time_steps):
            # Write and wait until finished, otherwise the writes will trip over each other
            lfr.write_array(arrays_written[t], t, array_pathname).get()

        for t in range(nr_time_steps):
            array_read = lfr.read_array(
                array_pathname, t, partition_shape=partition_shape
            )

            self.assertTrue(lfr.all(array_read == arrays_written[t]).future.get())
