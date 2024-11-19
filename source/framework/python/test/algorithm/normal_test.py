import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class NormalTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overload1(self):
        array_shape = (60, 40)
        mean = 33
        stddev = 55
        fill_value = 5

        for element_type in lfr.arithmetic_element_types:
            array = lfr.create_array(array_shape, element_type, fill_value)

            for result_element_type in lfr.floating_point_element_types:
                _ = lfr.normal(array, result_element_type, mean, stddev)

    @lue_test.framework_test_case
    def test_overload2(self):
        array_shape = (60, 40)
        mean = 33
        stddev = 55

        for element_type in lfr.floating_point_element_types:
            lfr.normal(array_shape, element_type, mean, stddev)
