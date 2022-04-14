import lue.framework as lfr
import unittest


def start_hpx_runtime():
    lfr.start_hpx_runtime(lfr.default_hpx_runtime_configuration)


def stop_hpx_runtime():
    lfr.stop_hpx_runtime()


class TestCase(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass
