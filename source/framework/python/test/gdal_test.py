import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ToNumPyTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_round_trip1(self):
        # Use default partition shape

        array_shape = (600, 400)
        dtype = np.int32
        fill_value = 5
        array_written = lfr.create_array(array_shape, dtype, fill_value)

        name = "to_gdal_round_trip1.tif"

        written = lfr.to_gdal(array_written, name)
        written.wait()  # Caveat! Don't read before the raster is written!

        array_read = lfr.from_gdal(name)

        self.assertEqual(array_read.dtype, array_written.dtype)
        self.assertEqual(array_read.shape, array_written.shape)
        self.assertTrue(lfr.all(array_read == array_written).future.get())

    @lue_test.framework_test_case
    def test_round_trip2(self):
        # Use specific partition shape

        array_shape = (600, 400)
        partition_shape = (10, 10)
        dtype = np.int32
        fill_value = 5
        array_written = lfr.create_array(
            array_shape, dtype, fill_value, partition_shape=partition_shape
        )

        name = "to_gdal_round_trip2.tif"

        written = lfr.to_gdal(array_written, name)
        written.wait()  # Caveat! Don't read before the raster is written!

        array_read = lfr.from_gdal(name, partition_shape=partition_shape)

        self.assertEqual(array_read.dtype, array_written.dtype)
        self.assertEqual(array_read.shape, array_written.shape)
        self.assertTrue(lfr.all(array_read == array_written).future.get())
