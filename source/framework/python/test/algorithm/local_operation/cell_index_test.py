import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class CellIndexTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        array_shape = (60, 40)
        condition = lfr.create_array(array_shape, lfr.boolean_element_type, 1)

        self.assert_overload(lfr.cell_index, condition, 0)
