import lue.framework as lfr
import lue_test
import numpy as np


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ToNumPyTest(lue_test.TestCase):

    @lue_test.framework_test_case
    def test_round_trip(self):

        array_shape = (600, 400)
        partition_shape = (10, 10)
        dtype = np.int32
        fill_value = 5
        array_written = lfr.create_array(array_shape, partition_shape, dtype, fill_value)

        name = "to_gdal_round_trip.tif"

        written = lfr.to_gdal(array_written, name)
        written.wait()  # Caveat! Don't read before the raster is written!

        array_read = lfr.from_gdal(name, partition_shape)

        self.assertEqual(array_read.dtype, array_written.dtype)
        self.assertEqual(array_read.shape, array_written.shape)
        self.assertTrue(lfr.all(array_read == array_written).get())
