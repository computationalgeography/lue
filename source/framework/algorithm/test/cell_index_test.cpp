#define BOOST_TEST_MODULE lue framework algorithm cell_index
#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


template<typename ConditionElement, typename IndexElement>
void test_cell_index(
    lue::Shape<lue::Count, 2> const& array_shape,
    lue::Shape<lue::Count, 2> const& partition_shape,
    std::initializer_list<std::initializer_list<ConditionElement>> const& condition_elements,
    std::initializer_list<std::initializer_list<IndexElement>> const& cell_index_elements,
    IndexElement const index)
{
    auto const condition = lue::test::create_partitioned_array<lue::PartitionedArray<ConditionElement, 2>>(
        array_shape, partition_shape, condition_elements);
    auto const result_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<IndexElement, 2>>(
        array_shape, partition_shape, cell_index_elements);
    auto const result_we_got = lue::value_policies::cell_index<IndexElement>(condition, index);
    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(single_partition)
{
    using ConditionElement = lue::BooleanElement;
    using IndexElement = lue::IndexElement;

    auto const cx{lue::policy::no_data_value<ConditionElement>};
    auto const ix{lue::policy::no_data_value<IndexElement>};

    test_cell_index<ConditionElement, IndexElement>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            0, cx, 1,
            1, 0, 1,
            1, cx, 0,
        }},
        {{
            ix, ix, 0,
            1, ix, 1,
            2, ix, ix,
        }},  // clang-format on
        0);

    test_cell_index<ConditionElement, IndexElement>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            0, cx, 1,
            1, 0, 1,
            1, cx, 0,
        }},
        {{
            ix, ix, 2,
            0, ix, 2,
            0, ix, ix,
        }},  // clang-format on
        1);
}


BOOST_AUTO_TEST_CASE(multiple_partitions)
{
    using ConditionElement = lue::BooleanElement;
    using IndexElement = lue::IndexElement;

    auto const cx{lue::policy::no_data_value<ConditionElement>};
    auto const ix{lue::policy::no_data_value<IndexElement>};

    test_cell_index<ConditionElement, IndexElement>(
        {6, 6},
        {3, 3},
        // clang-format off
        {
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
        },
        {
            {
                ix, ix, 0,
                1, ix, 1,
                2, ix, ix,
            },
            {
                ix, ix, 0,
                1, ix, 1,
                2, ix, ix,
            },
            {
                ix, ix, 3,
                4, ix, 4,
                5, ix, ix,
            },
            {
                ix, ix, 3,
                4, ix, 4,
                5, ix, ix,
            },
        },  // clang-format on
        0);

    test_cell_index<ConditionElement, IndexElement>(
        {6, 6},
        {3, 3},
        // clang-format off
        {
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
            {
                0, cx, 1,
                1, 0, 1,
                1, cx, 0,
            },
        },
        {
            {
                ix, ix, 2,
                0, ix, 2,
                0, ix, ix,
            },
            {
                ix, ix, 5,
                3, ix, 5,
                3, ix, ix,
            },
            {
                ix, ix, 2,
                0, ix, 2,
                0, ix, ix,
            },
            {
                ix, ix, 5,
                3, ix, 5,
                3, ix, ix,
            },
        },  // clang-format on
        1);
}
