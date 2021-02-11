import lue.framework as lfr
import unittest


class TestCase(unittest.TestCase):

    def setUp(self):
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


    def tearDown(self):

        lfr.stop_hpx_runtime()
