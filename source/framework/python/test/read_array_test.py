import numpy as np

import lue.data_model as ldm
import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ReadArrayTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_read_constant_array(self):

        # Create a dataset containing a constant raster
        dataset_pathname = "read_constant_array.lue"
        phenomenon_name = "earth"
        property_set_name = "continent"
        layer_name = "elevation"
        dtype = np.int32
        array_shape = (60, 40)
        space_box = [0, 0, 6000, 4000]
        fill_value = 5

        dataset = ldm.create_dataset(dataset_pathname)
        raster_view = ldm.hl.create_raster_view(
            dataset, phenomenon_name, property_set_name, array_shape, space_box
        )
        layer = raster_view.add_layer(layer_name, dtype)
        layer[:] = np.full(array_shape, fill_value, dtype)

        del layer, raster_view, dataset

        array_pathname = "{}/{}/{}/{}".format(
            dataset_pathname, phenomenon_name, property_set_name, layer_name
        )

        # Read and verify the array
        partition_shape = (10, 10)
        array = lfr.read_array(array_pathname)

        self.assertEqual(array.dtype, dtype)
        self.assertEqual(array.shape, array_shape)
        self.assertTrue(lfr.all(array == fill_value).get())

    @lue_test.framework_test_case
    def test_read_variable_array(self):

        # Create an in-memory dataset containing a variable raster
        dataset_pathname = "read_variable_array.lue"
        phenomenon_name = "earth"
        property_set_name = "continent"
        layer_name = "elevation"
        dtype = np.int32
        array_shape = (60, 40)
        space_box = [0, 0, 6000, 4000]
        fill_value = 5

        epoch = ldm.Epoch(
            ldm.Epoch.Kind.common_era, "2021-02-09", ldm.Calendar.gregorian
        )
        clock = ldm.Clock(epoch, ldm.Unit.day, 1)
        nr_time_steps = 7
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
        layer = raster_view.add_layer(layer_name, dtype)

        # Write a 3D array with rasters for all time steps
        layer[0:] = np.full((7,) + array_shape, fill_value, dtype)

        # TODO Something like this must work also!
        # for t in range(nr_time_steps):
        #     layer[0:t:] = np.full(array_shape, t * fill_value, dtype)

        del layer, raster_view, dataset

        array_pathname = "{}/{}/{}/{}".format(
            dataset_pathname, phenomenon_name, property_set_name, layer_name
        )

        # Read and verify the array
        partition_shape = (10, 10)
        array = lfr.read_array(array_pathname, time_step_idx=3)

        self.assertEqual(array.dtype, dtype)
        self.assertEqual(array.shape, array_shape)
        self.assertTrue(lfr.all(array == fill_value).get())

    @lue_test.framework_test_case
    def test_read_array_subset(self):

        # Create a dataset containing a constant raster
        dataset_pathname = "read_subset.lue"
        phenomenon_name = "earth"
        property_set_name = "continent"
        layer_name = "elevation"
        dtype = np.int32
        array_shape = (60, 40)
        space_box = [0, 0, 6000, 4000]

        dataset = ldm.create_dataset(dataset_pathname)
        raster_view = ldm.hl.create_raster_view(
            dataset, phenomenon_name, property_set_name, array_shape, space_box
        )
        layer = raster_view.add_layer(layer_name, dtype)
        array_written = np.arange(60 * 40, dtype=dtype).reshape(array_shape)
        layer[:] = array_written

        del layer, raster_view, dataset

        array_pathname = "{}/{}/{}/{}".format(
            dataset_pathname, phenomenon_name, property_set_name, layer_name
        )

        # Read a subset and verify the array
        subset_center = (30, 20)
        subset_shape = (25, 25)
        partition_shape = (10, 10)
        array_read = lfr.to_numpy(
            lfr.read_array(
                array_pathname,
                subset_center,
                subset_shape,
                partition_shape=partition_shape,
            )
        )

        self.assertEqual(array_read.dtype, dtype)
        np.testing.assert_array_equal(
            array_read, array_written[18 : 18 + 25, 8 : 8 + 25]
        )
