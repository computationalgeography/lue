import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class GdalTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_array_default_partition_shape(self):
        array_shape = (600, 400)
        dtype = np.int32
        fill_value = 5
        array_written = lfr.create_array(array_shape, dtype, fill_value)

        name = "gdal_array_default_partition_shape.tif"

        written = lfr.to_gdal(array_written, name)
        written.get()  # Caveat! Don't read before the raster is written!

        array_read = lfr.from_gdal(name)

        self.assertEqual(array_read.dtype, array_written.dtype)
        self.assertEqual(array_read.shape, array_written.shape)
        self.assertTrue(lfr.all(array_read == array_written).future.get())

    @lue_test.framework_test_case
    def test_array_custom_partition_shape(self):
        array_shape = (600, 400)
        partition_shape = (10, 10)
        dtype = np.int32
        fill_value = 5
        array_written = lfr.create_array(
            array_shape, dtype, fill_value, partition_shape=partition_shape
        )

        name = "gdal_array_custom_partition_shape.tif"

        written = lfr.to_gdal(array_written, name)
        written.get()  # Caveat! Don't read before the raster is written!

        array_read = lfr.from_gdal(name, partition_shape=partition_shape)

        self.assertEqual(array_read.dtype, array_written.dtype)
        self.assertEqual(array_read.shape, array_written.shape)
        self.assertTrue(lfr.all(array_read == array_written).future.get())

    @lue_test.framework_test_case
    def test_hyperslab(self):
        array_shape = (600, 400)

        condition = lfr.create_array(array_shape, lfr.boolean_element_type, 1)
        row_idxs_written = lfr.cell_index(condition, 0)
        col_idxs_written = lfr.cell_index(condition, 1)

        row_idxs_name = "gdal_hyperslab_row_idxs.tif"
        written = lfr.to_gdal(row_idxs_written, row_idxs_name)
        written.get()

        col_idxs_name = "gdal_hyperslab_col_idxs.tif"
        written = lfr.to_gdal(col_idxs_written, col_idxs_name)
        written.get()

        hyperslab_shape = (200, 100)
        hyperslab = lfr.Hyperslab(center=(300, 200), shape=hyperslab_shape)

        row_idxs_read = lfr.from_gdal(row_idxs_name, hyperslab=hyperslab)
        col_idxs_read = lfr.from_gdal(col_idxs_name, hyperslab=hyperslab)

        self.assertEqual(row_idxs_read.dtype, row_idxs_written.dtype)
        self.assertEqual(col_idxs_read.dtype, col_idxs_written.dtype)

        self.assertEqual(row_idxs_read.shape, hyperslab_shape)
        self.assertEqual(col_idxs_read.shape, hyperslab_shape)

        # Create arrays that contain the same values as the ones in the hyperslabs read from the raster
        condition = lfr.create_array(hyperslab_shape, lfr.boolean_element_type, 1)
        row_idxs_hyperslab = lfr.cell_index(condition, 0) + hyperslab.offsets[0]
        col_idxs_hyperslab = lfr.cell_index(condition, 1) + hyperslab.offsets[1]

        # Verify the hyperslabs read contain values we expect
        self.assertTrue(lfr.all(row_idxs_read == row_idxs_hyperslab).future.get())
        self.assertTrue(lfr.all(col_idxs_read == col_idxs_hyperslab).future.get())

    @lue_test.framework_test_case
    def test_creation_options(self):
        array_shape = (600, 400)
        dtype = np.int32
        fill_value = 5
        array_written = lfr.create_array(array_shape, dtype, fill_value)

        name = "gdal_creation_options.tif"
        options = {
            "BIGTIFF": "YES",
            "COMPRESS": "ZSTD",
        }

        written = lfr.to_gdal(array_written, name, options=options)
        written.get()
