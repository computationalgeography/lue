import numpy as np

import lue.framework as lfr
import lue.framework_x as lfrx
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class AccuTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_accu_raster_raster(self):
        # TODO
        pass
