import lue.framework as lfr
import lue_test
import numpy as np


class CreateArrayTest(lue_test.TestCase):

    def test_create_array(self):
        cfg = [
            # Make sure hpx_main is always executed
            "hpx.run_hpx_main!=1",
            # Allow for unknown command line options
            "hpx.commandline.allow_unknown!=1",
            # Disable HPX' short options
            "hpx.commandline.aliasing!=0",
            # Don't print diagnostics during forced terminate
            "hpx.diagnostics_on_terminate!=0",
            # Make AGAS clean up resources faster than by default
            "hpx.agas.max_pending_refcnt_requests!=50",
        ]
        lfr.start_hpx_runtime(cfg)
        array_shape = (600, 400)
        partition_shape = (10, 10)
        dtype = np.dtype(np.float32)
        array = lfr.create_array(array_shape, partition_shape, dtype)

        self.assertEqual(array.shape, array_shape)
        shape_in_partitions = (60, 40)
        self.assertEqual(array.shape_in_partitions, shape_in_partitions)
        lfr.stop_hpx_runtime()
