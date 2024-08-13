import numpy as np

import lue.framework_x as lfrx
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class IOTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_from_gdal(self):
        name = "x"
        partition_shape = (10, 10)

        # TODO
        # lfrx.from_gdal(name)
        # lfrx.from_gdal(name, partition_shape)
