import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class UniqueIDTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        # Use a small array. The element type may be a single byte integer.
        # TODO Fix unique_id to write no-data when the ID ends up being out of range.
        array_shape = (12, 8)
        fill_value = 1

        array = lfr.create_array(array_shape, lfr.boolean_element_type, fill_value)

        for element_type in lfr.integral_element_types:
            lfr.unique_id(array, element_type)
