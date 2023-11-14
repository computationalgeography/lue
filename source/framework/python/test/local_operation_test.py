# import lue.framework as lfr
import numpy as np

import lue_test


# import datetime


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class LocalOperationTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_sqrt(self):
        partitions = (100,)
        values = np.random.random(1000000)
        # partitioned_array = lfr.create_array(values, partitions)

        # start = datetime.datetime.now()
        # result_we_got = lfr.sqrt(partitioned_array)
        # stop = datetime.datetime.now()
        # # print("lue  : {}".format(stop - start))

        # start = datetime.datetime.now()
        # result_we_want = np.sqrt(values)
        # stop = datetime.datetime.now()

        # print("numpy: {}".format(stop - start))

        # result_we_got = lfr.sqrt(partitioned_array)
        # result_we_want = np.sqrt(values)
        # np.testing.assert_equal(result_we_got, result_we_want)
