#define BOOST_TEST_MODULE lue framework algorithm partition_count_unique
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/partition_count_unique.hpp"
#include "lue/framework/algorithm/unique_id.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using Element = std::int32_t;
    constexpr lue::Rank rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{60, 40}};
    Shape const partition_shape{{10, 10}};

    using Count = lue::Count;
    using OutputArray = lue::PartitionedArray<Count, rank>;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(same_values)
{
    // Create an array filled with the same value. In the result,
    // for each partition there must be a 1 stored.

    Array array{array_shape, partition_shape};
    lue::fill(array, 5).get();

    OutputArray result_we_want{lue::shape_in_partitions(array), {{1, 1}}};
    lue::fill(result_we_want, Count{1}).get();

    auto result_we_got = lue::partition_count_unique(array);

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(different_values)
{
    // Create an array filled with different values. In the result,
    // for each partition there must be a value stored equal to the
    // number of elements in each partition.

    Array array{array_shape, partition_shape};
    lue::unique_id(array).get();

    OutputArray result_we_want{lue::shape_in_partitions(array), {{1, 1}}};
    lue::fill(result_we_want, Count{lue::nr_elements(partition_shape)}).get();

    auto result_we_got = lue::partition_count_unique(array);

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    // Create an array filled with no-data. In the result, for each
    // partition there must be a no-data value stored.

    Array array{array_shape, partition_shape};
    lue::fill(array, lue::policy::no_data_value<Element>).get();

    OutputArray result_we_want{lue::shape_in_partitions(array), {{1, 1}}};
    lue::fill(result_we_want, Count{lue::policy::no_data_value<Count>}).get();

    // TODO Needs ValuePolicies
    BOOST_WARN(false);

    // using Policies = lue::policy::partition_count_unique::DefaultValuePolicies<Count, Element>;
    // auto result_we_got = lue::partition_count_unique(policies, array);

    // lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}
