#define BOOST_TEST_MODULE lue framework algorithm round
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/round.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(pcraster_manual_example1)
{
    auto const nd{lue::no_data<float>};
    lue::Shape<lue::Count, 2> const array_shape{3, 3};
    lue::Shape<lue::Count, 2> const partition_shape{3, 3};

    auto const expression = lue::test::create_partitioned_array<lue::PartitionedArray<float, 2>>(
        array_shape,
        partition_shape,
        {{
            0.0,
            1.5,
            3.4,
            nd,
            -3.2,
            -2.5,
            0.01,
            9.3,
            8.9,
        }});

    auto const result_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<float, 2>>(
        array_shape,
        partition_shape,
        {{
            0.0,
            2.0,
            3.0,
            nd,
            -3.0,
            -3.0,
            0.0,
            9.0,
            9.0,
        }});

    auto const result_we_got = lue::value_policies::round(expression);

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}
