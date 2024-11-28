import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class WaitTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        for element_type in lfr.arithmetic_element_types:
            array = lfr.create_array(array_shape, element_type, 5)
            lfr.wait(array)
