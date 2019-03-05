#define BOOST_TEST_MODULE lue framework algorithm unique_id
// #include "lue/framework/algorithm/unique_id.hpp"
// #include "lue/framework/algorithm/unique.hpp"
// #include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
// #include "lue/framework/test/stream.hpp"


// LUE_REGISTER_ARRAY_PARTITION(int32_t, 1)


BOOST_AUTO_TEST_CASE(array_1d)
{
    BOOST_CHECK(true);

    // using Value = std::int32_t;
    // std::size_t const rank = 1;
    // using Data = lue::ArrayPartitionData<Value, rank>;
    // using Array = lue::PartitionedArray<std::int32_t, Data>;
    // // using Definition = typename Array::Definition;
    // using Shape = typename Array::Shape;

    // typename Data::Index const nr_elements = 100;

    // Shape shape{Shape{{nr_elements}}};

    // // Create input array
    // lue::PartitionedArray<std::int32_t, Data> array{shape};

    // // Update array partitions such that all elements in the array
    // // contain a unique value
    // lue::unique_id(array);

    // // // Find the unique elements in an array
    // // auto const unique_elements = lue::unique(array);

    // // // Assert that the number of unique values equals the number of
    // // // elements in the original array
    // // auto const nr_elements = lue::size(unique_elements);
    // // BOOST_CHECK_EQUAL(
}
