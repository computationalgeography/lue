import numpy as np

import lue.framework as lfr
import lue.pcraster as lpr
import lue_test

from .operation_test import OperationTest, setUpModule, tearDownModule


class NumpyTest(OperationTest):
    @lue_test.framework_test_case
    def test_pcr2numpy(self):
        for expression_type in [np.uint8, np.int32, np.float32]:
            spatial, non_spatial = (
                self.spatial[expression_type],
                self.non_spatial[expression_type],
            )

            lpr.pcr2numpy(spatial, no_data_value=non_spatial.future.get())

    @lue_test.framework_test_case
    def test_numpy2pcr(self):
        nr_rows = 10
        nr_cols = 20

        for data_type in [lpr.Boolean, lpr.Ldd]:
            expression_type = np.uint8
            array = np.ones(shape=(nr_rows, nr_cols), dtype=expression_type)
            _ = lpr.numpy2pcr(data_type, array, 2)

        for data_type in [lpr.Nominal, lpr.Ordinal]:
            expression_type = np.int32
            array = np.ones(shape=(nr_rows, nr_cols), dtype=expression_type)
            _ = lpr.numpy2pcr(data_type, array, 2)

        for data_type in [lpr.Scalar]:
            expression_type = np.float32
            array = np.ones(shape=(nr_rows, nr_cols), dtype=expression_type)
            _ = lpr.numpy2pcr(data_type, array, 2.0)
