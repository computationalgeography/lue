import lue.data_model as ldm
import lue.framework as lfr
import lue_test
import numpy as np


class ReadArrayTest(lue_test.TestCase):

    def test_read_constant_array(self):

        # Create an in-memory dataset containing a constant raster
        dataset_pathname = "read_constant_array.lue"
        phenomenon_name = "earth"
        property_set_name = "continent"
        layer_name = "elevation"
        dtype = np.dtype(np.int32)
        array_shape = (600, 400)
        space_box = [0, 0, 6000, 4000]
        fill_value = 5

        dataset = ldm.create_dataset(dataset_pathname)
        raster_view = ldm.hl.create_raster_view(
            dataset, phenomenon_name, property_set_name, array_shape, space_box)
        layer = raster_view.add_layer(layer_name, dtype)
        layer[:] = np.full(array_shape, fill_value, dtype)

        array_pathname = "{}/{}/{}/{}".format(
            dataset_pathname, phenomenon_name, property_set_name, layer_name)

        # Read and verify the array
        partition_shape = (10, 10)
        array = lfr.read_array(array_pathname, partition_shape)

        self.assertEqual(array.dtype, dtype)
        self.assertEqual(array.shape, array_shape)
        self.assertTrue(lfr.all(array == fill_value).get())


    def test_read_variable_array(self):

        # Create an in-memory dataset containing a variable raster
        dataset_pathname = "read_variable_array.lue"
        phenomenon_name = "earth"
        property_set_name = "continent"
        layer_name = "elevation"
        dtype = np.dtype(np.int32)
        array_shape = (600, 400)
        space_box = [0, 0, 6000, 4000]
        fill_value = 5

        epoch = ldm.Epoch(
            ldm.Epoch.Kind.common_era, "2021-02-09", ldm.Calendar.gregorian)
        clock = ldm.Clock(epoch, ldm.Unit.day, 1)
        nr_time_steps = 7
        time_box = [0, 8]

        dataset = ldm.create_dataset(dataset_pathname)
        raster_view = ldm.hl.create_raster_view(
            dataset, phenomenon_name, property_set_name,
            clock, nr_time_steps, time_box, array_shape, space_box)
        layer = raster_view.add_layer(layer_name, dtype)

        # Write a 3D array with rasters for all time steps
        layer[0:] = np.full((7,) + array_shape, fill_value, dtype)

        # TODO Something like this must work also!
        # for t in range(nr_time_steps):
        #     layer[0:t:] = np.full(array_shape, t * fill_value, dtype)

        array_pathname = "{}/{}/{}/{}".format(
            dataset_pathname, phenomenon_name, property_set_name, layer_name)

        # Read and verify the array
        partition_shape = (10, 10)
        array = lfr.read_array(array_pathname, partition_shape, time_step_idx=3)

        self.assertEqual(array.dtype, dtype)
        self.assertEqual(array.shape, array_shape)
        self.assertTrue(lfr.all(array == fill_value).get())
