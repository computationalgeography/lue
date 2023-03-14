import numpy as np

import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class ReclassifyTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):

        array_shape = (60, 40)
        partition_shape = (10, 10)
        id_ = 3

        for from_type in [np.uint8, np.uint32, np.uint64, np.int32, np.int64]:
            ids = lfr.create_array(array_shape, partition_shape, from_type, id_)

            for to_type in [np.float32, np.float64]:
                lookup_table = {
                    1: 1.1,
                    2: 2.2,
                    3: 3.3,
                    4: 4.4,
                }
                array = lfr.reclassify(ids, lookup_table, dtype=to_type)

                self.assertEqual(array.dtype, np.dtype(to_type))
