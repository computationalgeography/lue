import lue.framework as lfr
import lue_test


def setUpModule():
    lue_test.start_hpx_runtime()


def tearDownModule():
    lue_test.stop_hpx_runtime()


class AccuFractionTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)

        direction = 2
        flow_direction = lfr.create_array(
            array_shape, lfr.flow_direction_element_type, direction
        )

        for element_type in lfr.floating_point_element_types:
            material = lfr.create_array(array_shape, element_type, 1)
            fraction = lfr.create_array(array_shape, element_type, 0.8)
            flux, state = lfr.accu_fraction(flow_direction, material, fraction)
