import lue.framework as lfr
import lue_test
from lue_test.operation_test import OperationTest, setUpModule, tearDownModule


class ArrayPartitionIDTest(OperationTest):
    @lue_test.framework_test_case
    def test_overloads(self):
        if lfr.with_development_operations:
            array_shape = (60, 40)

            for element_type in lfr.arithmetic_element_types:
                array = lfr.create_array(array_shape, element_type, fill_value=5)
                self.assert_overload(lfr.array_partition_id, array)
