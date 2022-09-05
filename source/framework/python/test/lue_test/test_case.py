import lue.framework as lfr
import unittest


def start_hpx_runtime():
    # print(os.environ["LUE_HPX_RUNTIME_CONFIGURATION"])
    # print(lfr.default_hpx_runtime_configuration)
    lfr.start_hpx_runtime(lfr.default_hpx_runtime_configuration)
    # time.sleep(1)


def stop_hpx_runtime():
    lfr.stop_hpx_runtime()


def framework_test_case(
        test_case):

    def wrapper(self):
        if lfr.on_root_locality():
            test_case(self)

    return wrapper


class TestCase(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass
