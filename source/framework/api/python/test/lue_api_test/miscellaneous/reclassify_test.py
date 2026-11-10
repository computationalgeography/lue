import lue_test
from lue_test.operation_test import setUpModule, tearDownModule

import lue.framework_x as lfrx


class ReclassifyTest(lue_test.TestCase):
    @lue_test.framework_test_case
    def test_reclassify(self):

        pass

        # array_shape = (60, 40)
        # partition_shape = (6, 4)
        # input_element_type = lfrx.integral_element_types[-1]
        # output_element_type = lfrx.floating_point_element_types[-1]
        #
        # input_scalar = lfrx.create_scalar(5, input_element_type)
        # input_array = lfrx.create_array(
        #     array_shape, fill_value=input_scalar, partition_shape=partition_shape
        # )
        # input_field = lfrx.as_field(input_array)

        # TODO: finish. We need a create_lookup_table or so, to make the element types configurable.
        # lookup_table = create_lookup_table(from_element_type=x, to_element_type=y, table=z)
        # lookup_table = {5: 5.5}
        #
        # result = lfrx.reclassify(input_field, lookup_table)
        #
        # self.assertEqual(result.data_model, lfrx.DataModel.Array)
        # self.assertEqual(result.dtype, lfrx.output_element_type)
