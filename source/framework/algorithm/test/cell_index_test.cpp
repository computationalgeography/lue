#define BOOST_TEST_MODULE lue framework algorithm cell_index
#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(cell_index)
{
    using namespace lue::value_policies;

    lue::Shape<lue::Count, 2> const array_shape{3, 3};
    lue::Shape<lue::Count, 2> const partition_shape{3, 3};

    using ConditionElement = std::uint8_t;
    using IndexElement = std::uint64_t;

    auto const condition_nd{lue::no_data<ConditionElement>};
    auto const index_nd{lue::no_data<IndexElement>};

    auto const condition = lue::test::create_partitioned_array<lue::PartitionedArray<ConditionElement, 2>>(
        array_shape,
        partition_shape,
        {{
            0,
            condition_nd,
            1,
            1,
            0,
            1,
            1,
            condition_nd,
            0,
        }});

    {
        auto const result_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<IndexElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    index_nd,
                    index_nd,
                    0,
                    1,
                    index_nd,
                    1,
                    2,
                    index_nd,
                    index_nd,
                }});

        auto const result_we_got = lue::value_policies::cell_index<IndexElement>(condition, 0);

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }

    {
        auto const result_we_want =
            lue::test::create_partitioned_array<lue::PartitionedArray<IndexElement, 2>>(
                array_shape,
                partition_shape,
                {{
                    index_nd,
                    index_nd,
                    2,
                    0,
                    index_nd,
                    2,
                    0,
                    index_nd,
                    index_nd,
                }});

        auto const result_we_got = lue::value_policies::cell_index<IndexElement>(condition, 1);

        lue::test::check_arrays_are_equal(result_we_got, result_we_want);
    }
}
