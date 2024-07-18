#define BOOST_TEST_MODULE lue framework algorithm clump
#include "lue/framework/algorithm/value_policies/clump.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/algorithm/value_policies/unique.hpp"
#include "lue/framework/algorithm/value_policies/where.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <random>


template<typename Element>
void test_clump(
    auto const& array_shape,
    auto const& partition_shape,
    std::initializer_list<std::initializer_list<Element>> const& zone_elements,
    std::initializer_list<std::initializer_list<Element>> const& clump_elements,
    lue::Connectivity const connectivity)
{
    auto const zone = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        array_shape, partition_shape, zone_elements);
    auto const result_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        array_shape, partition_shape, clump_elements);
    auto const result_we_got = lue::value_policies::clump(zone, connectivity);
    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


template<typename Element>
void test_clump(
    lue::Shape<lue::Count, 2> const& array_shape,
    lue::Shape<lue::Count, 2> const& partition_shape,
    std::initializer_list<std::initializer_list<Element>> const& zone_elements,
    std::initializer_list<std::initializer_list<Element>> const& clump_elements_nondiagonal,
    std::initializer_list<std::initializer_list<Element>> const& clump_elements_diagonal)
{
    BOOST_TEST_CONTEXT("nondiagonal")
    {
        test_clump(
            array_shape,
            partition_shape,
            zone_elements,
            clump_elements_nondiagonal,
            lue::Connectivity::nondiagonal);
    }
    BOOST_TEST_CONTEXT("diagonal")
    {
        test_clump(
            array_shape,
            partition_shape,
            zone_elements,
            clump_elements_diagonal,
            lue::Connectivity::diagonal);
    }
}


BOOST_AUTO_TEST_CASE(constant_1x1)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            5, 5, 5,
            5, 5, 5,
            5, 5, 5,
        }},
        {{
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
        }},
        {{
            0, 0, 0,
            0, 0, 0,
            0, 0, 0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(constant_2x2)
{
    test_clump<std::int32_t>(
        {6, 6},
        {3, 3},
        // clang-format off
        {
            { 5, 5, 5, 5, 5, 5, 5, 5, 5, },
            { 5, 5, 5, 5, 5, 5, 5, 5, 5, },
            { 5, 5, 5, 5, 5, 5, 5, 5, 5, },
            { 5, 5, 5, 5, 5, 5, 5, 5, 5, },
        },
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
        },
        {
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
            { 0, 0, 0, 0, 0, 0, 0, 0, 0, },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    auto const x{lue::policy::no_data_value<std::int32_t>};

    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            x, x, x,
            x, x, x,
            x, x, x,
        }},
        {{
            x, x, x,
            x, x, x,
            x, x, x,
        }},
        {{
            x, x, x,
            x, x, x,
            x, x, x,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(increasing_range_1x1)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
        }},
        {{
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
        }},
        {{
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(increasing_range_1x2)
{
    test_clump<std::int32_t>(
        {3, 6},
        {3, 3},
        // clang-format off
        {
            {
                 0,  1,  2,
                 6,  7,  8,
                12, 13, 14,
            },
            {
                 3,  4,  5,
                 9, 10, 11,
                15, 16, 17,
            },
        },
        {
            {
                 0,  1,  2,
                 3,  4,  5,
                 6,  7,  8,
            },
            {
                 9, 10, 11,
                12, 13, 14,
                15, 16, 17,
            },
        },
        {
            {
                 0,  1,  2,
                 3,  4,  5,
                 6,  7,  8,
            },
            {
                 9, 10, 11,
                12, 13, 14,
                15, 16, 17,
            },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(increasing_range_2x1)
{
    test_clump<std::int32_t>(
        {6, 3},
        {3, 3},
        // clang-format off
        {
            {
                 0,  1,  2,
                 3,  4,  5,
                 6,  7,  8,
            },
            {
                 9, 10, 11,
                12, 13, 14,
                15, 16, 17,
            },
        },
        {
            {
                 0,  1,  2,
                 3,  4,  5,
                 6,  7,  8,
            },
            {
                 9, 10, 11,
                12, 13, 14,
                15, 16, 17,
            },
        },
        {
            {
                 0,  1,  2,
                 3,  4,  5,
                 6,  7,  8,
            },
            {
                 9, 10, 11,
                12, 13, 14,
                15, 16, 17,
            },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(decreasing_range)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            8, 7, 6,
            5, 4, 3,
            2, 1, 0,
        }},
        {{
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
        }},
        {{
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(seed_to_south_west)
{
    auto const x{lue::policy::no_data_value<std::int32_t>};

    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            x, x, x,
            x, 5, x,
            5, x, x,
        }},
        {{
            x, x, x,
            x, 0, x,
            1, x, x,
        }},
        {{
            x, x, x,
            x, 0, x,
            0, x, x,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(seed_to_south_east)
{
    auto const x{lue::policy::no_data_value<std::int32_t>};

    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            x, x, x,
            x, 5, x,
            x, x, 5,
        }},
        {{
            x, x, x,
            x, 0, x,
            x, x, 1,
        }},
        {{
            x, x, x,
            x, 0, x,
            x, x, 0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(checker_board_1)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            5, 6, 5,
            6, 5, 6,
            5, 6, 5,
        }},
        {{
            0, 1, 2,
            3, 4, 5,
            6, 7, 8,
        }},
        {{
            0, 1, 0,
            1, 0, 1,
            0, 1, 0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(checker_board_2)
{
    auto const x{lue::policy::no_data_value<std::int32_t>};

    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            5, x, 5,
            x, 5, x,
            5, x, 5,
        }},
        {{
            0, x, 1,
            x, 2, x,
            3, x, 4,
        }},
        {{
            0, x, 0,
            x, 0, x,
            0, x, 0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(checker_board_3)
{
    auto const x{lue::policy::no_data_value<std::int32_t>};

    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            x, 6, x,
            6, x, 6,
            x, 6, x,
        }},
        {{
            x, 0, x,
            1, x, 2,
            x, 3, x,
        }},
        {{
            x, 0, x,
            0, x, 0,
            x, 0, x,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(rows)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            1, 1, 1,
            2, 2, 2,
            3, 3, 3,
        }},
        {{
            0, 0, 0,
            1, 1, 1,
            2, 2, 2,
        }},
        {{
            0, 0, 0,
            1, 1, 1,
            2, 2, 2,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(cols)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            1, 2, 3,
            1, 2, 3,
            1, 2, 3,
        }},
        {{
            0, 1, 2,
            0, 1, 2,
            0, 1, 2,
        }},
        {{
            0, 1, 2,
            0, 1, 2,
            0, 1, 2,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(diagonal_1)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            1, 2, 2,
            3, 1, 2,
            3, 3, 1,
        }},
        {{
            0, 1, 1,
            2, 3, 1,
            2, 2, 4,
        }},
        {{
            0, 1, 1,
            2, 0, 1,
            2, 2, 0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(diagonal_2)
{
    test_clump<std::int32_t>(
        {3, 3},
        {3, 3},
        // clang-format off
        {{
            2, 2, 1,
            2, 1, 3,
            1, 3, 3,
        }},
        {{
            0, 0, 1,
            0, 2, 3,
            4, 3, 3,
        }},
        {{
            0, 0, 1,
            0, 1, 2,
            1, 2, 2,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(diagonal_2x2)
{
    test_clump<std::int32_t>(
        {6, 6},
        {3, 3},
        // clang-format off
        {
            { 5, 5, 5,
              5, 5, 5,
              5, 5, 6, },
            { 5, 5, 5,
              5, 5, 5,
              7, 5, 5, },
            { 5, 5, 7,
              5, 5, 5,
              5, 5, 5, },
            { 6, 5, 5,
              5, 5, 5,
              5, 5, 5, },
        },
        {
            { 0, 0, 0,
              0, 0, 0,
              0, 0, 1, },
            { 0, 0, 0,
              0, 0, 0,
              3, 0, 0, },
            { 0, 0, 5,
              0, 0, 0,
              0, 0, 0, },
            { 6, 0, 0,
              0, 0, 0,
              0, 0, 0, },
        },
        {
            { 0, 0, 0,
              0, 0, 0,
              0, 0, 1, },
            { 0, 0, 0,
              0, 0, 0,
              3, 0, 0, },
            { 0, 0, 3,
              0, 0, 0,
              0, 0, 0, },
            { 1, 0, 0,
              0, 0, 0,
              0, 0, 0, },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(diagonal_3x3)
{
    test_clump<std::int32_t>(
        {9, 9},
        {3, 3},
        // clang-format off
        {
            {  6,  5,  5,
               5,  6,  5,
               5,  5,  6, },
            {  5,  5,  5,
               5,  5,  5,
               5,  5,  5, },
            {  5,  5,  7,
               5,  7,  5,
               7,  5,  5, },
            {  5,  5,  5,
               5,  5,  5,
               5,  5,  5, },
            {  6,  5,  7,
               5,  6,  7,
               7,  7,  6, },
            {  5,  5,  5,
               5,  5,  5,
               5,  5,  5, },
            {  5,  5,  7,
               5,  7,  5,
               7,  5,  5, },
            {  5,  5,  5,
               5,  5,  5,
               5,  5,  5, },
            {  6,  5,  5,
               5,  6,  5,
               5,  5,  6, },
        },
        {
            {  0,  1,  1,
               2,  3,  1,
               2,  2,  4, },
            {  1,  1,  1,
               1,  1,  1,
               1,  1,  1, },
            {  1,  1,  7,
               1,  8,  9,
              10,  9,  9, },
            {  2,  2,  2,
               2,  2,  2,
               2,  2,  2, },
            { 12,  1, 14,
               2, 16, 14,
              17, 17, 18, },
            {  9,  9,  9,
               9,  9,  9,
               9,  9,  9, },
            {  2,  2, 21,
               2, 22, 23,
              24, 23, 23, },
            { 23, 23, 23,
              23, 23, 23,
              23, 23, 23, },
            { 26,  9,  9,
              23, 29,  9,
              23, 23, 30, },
        },
        {
            { 0, 1, 1,
              1, 0, 1,
              1, 1, 0, },
            { 1, 1, 1,
              1, 1, 1,
              1, 1, 1, },
            { 1, 1, 4,
              1, 4, 1,
              4, 1, 1, },
            { 1, 1, 1,
              1, 1, 1,
              1, 1, 1, },
            { 0, 1, 4,
              1, 0, 4,
              4, 4, 0, },
            { 1, 1, 1,
              1, 1, 1,
              1, 1, 1, },
            { 1, 1, 4,
              1, 4, 1,
              4, 1, 1, },
            { 1, 1, 1,
              1, 1, 1,
              1, 1, 1, },
            { 0, 1, 1,
              1, 0, 1,
              1, 1, 0, },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(reclass)
{
    test_clump<std::int32_t>(
        {6, 6},
        {3, 3},
        // clang-format off
        {
            {
                 0,  0,  1,
                 0,  0,  1,
                 3,  3,  4,
            },
            {
                 1,  0,  0,
                 1,  0,  0,
                 5,  6,  6,
            },
            {
                 3,  3,  5,
                 0,  0,  8,
                 0,  0,  8,
            },
            {
                 4,  6,  6,
                 8,  0,  0,
                 8,  0,  0,
            },
        },
        {
            {
                 0,  0,  1,
                 0,  0,  1,
                 2,  2,  3,
            },
            {
                 1,  5,  5,
                 1,  5,  5,
                 6,  7,  7,
            },
            {
                 2,  2,  9,
                10, 10, 11,
                10, 10, 11,
            },
            {
                12,  7,  7,
                11, 15, 15,
                11, 15, 15,
            },
        },
        {
            {
                 0,  0,  1,
                 0,  0,  1,
                 2,  2,  3,
            },
            {
                 1,  5,  5,
                 1,  5,  5,
                 6,  7,  7,
            },
            {
                 2,  2,  6,
                10, 10, 11,
                10, 10, 11,
            },
            {
                 3,  7,  7,
                11, 15, 15,
                11, 15, 15,
            },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(u_turn_1)
{
    test_clump<std::int32_t>(
        {3, 10},
        {3, 10},
        // clang-format off
        {{
            5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
            6, 5, 6, 5, 6, 5, 6, 5, 6, 5,
            5, 6, 5, 6, 5, 6, 5, 6, 5, 6,
        }},
        {{
             0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
            10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
            20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
        }},
        {{
            0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
            1, 0, 1, 0, 1, 0, 1, 0, 1, 0,
            0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(u_turn_2)
{
    test_clump<std::int32_t>(
        {3, 10},
        {3, 10},
        // clang-format off
        {{
            5, 5, 6, 6, 5, 5, 6, 6, 5, 5,
            6, 6, 5, 5, 6, 6, 5, 5, 6, 6,
            5, 5, 6, 6, 5, 5, 6, 6, 5, 5,
        }},
        {{
             0,  0,  1,  1,  2,  2,  3,  3,  4,  4,
             5,  5,  6,  6,  7,  7,  8,  8,  9,  9,
            10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
        }},
        {{
            0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
            1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
            0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(u_turn_3)
{
    test_clump<std::int32_t>(
        {3, 12},
        {3, 12},
        // clang-format off
        {{
            5, 5, 5, 6, 6, 6, 5, 5, 5, 6, 6, 6,
            6, 6, 6, 5, 5, 5, 6, 6, 6, 5, 5, 5,
            5, 5, 5, 6, 6, 6, 5, 5, 5, 6, 6, 6,
        }},
        {{
             0,  0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,
             4,  4,  4,  5,  5,  5,  6,  6,  6,  7,  7,  7,
             8,  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 11,
        }},
        {{
            0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1,
            1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0,
            0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_01)
{
    test_clump<std::int32_t>(
        {5, 5},
        {5, 5},
        // clang-format off
        {{
            2, 4, 1, 4, 5,
            1, 2, 3, 4, 4,
            1, 2, 5, 2, 2,
            4, 5, 2, 5, 2,
            4, 4, 1, 4, 2,
        }},
        {{
             0,  1,  2,  3,  4,
             5,  6,  7,  3,  3,
             5,  6,  8,  9,  9,
            10, 11, 12, 13,  9,
            10, 10, 14, 15,  9,
        }},
        {{
             0,  1,  2,  3,  4,
             5,  0,  6,  3,  3,
             5,  0,  7,  0,  0,
             8,  7,  0,  7,  0,
             8,  8,  9, 10,  0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_02)
{
    test_clump<std::int32_t>(
        {5, 5},
        {5, 5},
        // clang-format off
        {{
            1, 2, 2, 1, 1,
            1, 1, 1, 1, 1,
            1, 1, 2, 1, 1,
            1, 2, 1, 2, 1,
            1, 2, 1, 1, 1,
        }},
        {{
            0, 1, 1, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 2, 0, 0,
            0, 3, 0, 4, 0,
            0, 3, 0, 0, 0,
        }},
        {{
            0, 1, 1, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 2, 0, 0,
            0, 2, 0, 2, 0,
            0, 2, 0, 0, 0,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_03)
{
    test_clump<std::int32_t>(
        {5, 5},
        {5, 5},
        // clang-format off
        {{
            2, 2, 2, 2, 1,
            1, 2, 2, 2, 2,
            1, 2, 1, 2, 1,
            1, 2, 2, 2, 2,
            2, 1, 2, 2, 1,
        }},
        {{
            0, 0, 0, 0, 1,
            2, 0, 0, 0, 0,
            2, 0, 3, 0, 4,
            2, 0, 0, 0, 0,
            5, 6, 0, 0, 7,
        }},
        {{
            0, 0, 0, 0, 1,
            2, 0, 0, 0, 0,
            2, 0, 3, 0, 4,
            2, 0, 0, 0, 0,
            0, 2, 0, 0, 5,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_04)
{
    // U-turn back up
    // diagonal: cell (1, 1) must be 1, not 2
    test_clump<std::int32_t>(
        {4, 5},
        {4, 5},
        // clang-format off
        {{
            51, 51, 51, 51, 50,
            51, 50, 51, 50, 50,
            50, 51, 50, 50, 51,
            51, 50, 50, 51, 51,
        }},
        {{
            0, 0, 0, 0, 1,
            0, 2, 0, 1, 1,
            3, 4, 1, 1, 5,
            6, 1, 1, 5, 5,
        }},
        {{
            0, 0, 0, 0, 1,
            0, 1, 0, 1, 1,
            1, 0, 1, 1, 2,
            0, 1, 1, 2, 2,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_05)
{
    test_clump<std::int32_t>(
        {4, 10},
        {2, 10},
        // clang-format off
        {
            {
                51, 51, 51, 50, 50, 50, 51, 51, 50, 51,
                50, 51, 50, 50, 51, 50, 50, 50, 51, 51,
            },
            {
                50, 50, 51, 50, 50, 50, 51, 51, 51, 51,
                51, 51, 51, 51, 50, 51, 51, 50, 50, 51,
            },
        },
        {
            {
                 0,  0,  0,  1,  1,  1,  2,  2,  3,  4,
                 5,  0,  1,  1,  6,  1,  1,  1,  4,  4,
            },
            {
                 5,  5,  8,  1,  1,  1,  4,  4,  4,  4,
                 8,  8,  8,  8,  1,  4,  4, 11, 11,  4,
            },
        },
        {
            {
                 0,  0,  0,  1,  1,  1,  2,  2,  1,  2,
                 1,  0,  1,  1,  4,  1,  1,  1,  2,  2,
            },
            {
                 1,  1,  0,  1,  1,  1,  2,  2,  2,  2,
                 0,  0,  0,  0,  1,  2,  2,  9,  9,  2,
            },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_06)
{
    test_clump<std::int32_t>(
        {5, 10},
        {5, 10},
        // clang-format off
        {{
            51, 50, 50, 51, 51, 50, 51, 50, 50, 51,
            50, 50, 51, 50, 51, 51, 51, 51, 51, 50,
            51, 51, 50, 51, 50, 51, 50, 50, 50, 50,
            50, 50, 51, 50, 51, 50, 51, 51, 50, 50,
            50, 51, 50, 51, 50, 51, 50, 50, 50, 50,
        }},
        {{
             0,  1,  1,  2,  2,  3,  2,  4,  4,  5,
             1,  1,  6,  7,  2,  2,  2,  2,  2,  8,
             9,  9, 10, 11, 12,  2,  8,  8,  8,  8,
            13, 13, 14, 15, 16, 17, 18, 18,  8,  8,
            13, 19, 20, 21, 22, 23,  8,  8,  8,  8,
        }},
        {{
             0,  1,  1,  2,  2,  3,  2,  1,  1,  2,
             1,  1,  2,  1,  2,  2,  2,  2,  2,  1,
             2,  2,  1,  2,  1,  2,  1,  1,  1,  1,
             1,  1,  2,  1,  2,  1,  2,  2,  1,  1,
             1,  2,  1,  2,  1,  2,  1,  1,  1,  1,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_07)
{
    test_clump<std::int32_t>(
        {5, 10},
        {5, 10},
        // clang-format off
        {{
            50, 51, 51, 51, 50, 50, 50, 50, 51, 50,
            51, 50, 51, 50, 51, 51, 51, 50, 50, 51,
            51, 50, 50, 51, 51, 50, 50, 50, 50, 51,
            51, 51, 51, 50, 50, 50, 51, 51, 50, 51,
            50, 51, 50, 50, 51, 50, 50, 50, 51, 51,
        }},
        {{
             0,  1,  1,  1,  2,  2,  2,  2,  3,  4,
             5,  6,  1,  7,  8,  8,  8,  2,  2,  9,
             5,  6,  6,  8,  8,  2,  2,  2,  2,  9,
             5,  5,  5,  2,  2,  2, 10, 10,  2,  9,
            11,  5,  2,  2, 12,  2,  2,  2,  9,  9,
        }},
        {{
             0,  1,  1,  1,  0,  0,  0,  0,  2,  0,
             1,  0,  1,  0,  1,  1,  1,  0,  0,  2,
             1,  0,  0,  1,  1,  0,  0,  0,  0,  2,
             1,  1,  1,  0,  0,  0,  2,  2,  0,  2,
             3,  1,  0,  0,  4,  0,  0,  0,  2,  2,
        }}  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_08)
{
    test_clump<std::int32_t>(
        {20, 10},
        {5, 10},
        // clang-format off
        {
            {
                50, 50, 51, 50, 51, 51, 51, 51, 51, 50,
                51, 50, 51, 51, 51, 51, 50, 51, 50, 50,
                50, 50, 51, 50, 50, 50, 51, 50, 50, 51,
                50, 51, 51, 51, 51, 51, 50, 50, 51, 51,
                51, 50, 50, 51, 50, 51, 51, 51, 51, 51,
            },
            {
                51, 50, 50, 51, 51, 50, 51, 50, 50, 51,
                50, 50, 51, 50, 51, 51, 51, 51, 51, 50,
                51, 51, 50, 51, 50, 51, 50, 50, 50, 50,
                50, 50, 51, 50, 51, 50, 51, 51, 50, 50,
                50, 51, 50, 51, 50, 51, 50, 50, 50, 50,
            },
            {
                50, 51, 51, 51, 50, 50, 50, 50, 51, 50,
                51, 50, 51, 50, 51, 51, 51, 50, 50, 51,
                51, 50, 50, 51, 51, 50, 50, 50, 50, 51,
                51, 51, 51, 50, 50, 50, 51, 51, 50, 51,
                50, 51, 50, 50, 51, 50, 50, 50, 51, 51,
            },
            {
                50, 50, 51, 50, 50, 50, 51, 51, 51, 51,
                51, 51, 51, 51, 50, 51, 51, 50, 50, 51,
                50, 50, 50, 50, 50, 51, 51, 51, 51, 51,
                50, 50, 51, 51, 51, 51, 50, 51, 51, 50,
                51, 51, 50, 50, 51, 50, 51, 50, 50, 51,
            },
        },
        {
            {
                 0,  0,  1,  2,  1,  1,  1,  1,  1,  3,
                 4,  0,  1,  1,  1,  1,  5,  1,  3,  3,
                 0,  0,  1,  6,  6,  6,  7,  3,  3,  1,
                 0,  1,  1,  1,  1,  1,  3,  3,  1,  1,
                 8,  9,  9,  1, 10,  1,  1,  1,  1,  1,
            },
            {
                 8,  9,  9,  1,  1, 14,  1, 15, 15,  1,
                 9,  9, 17, 18,  1,  1,  1,  1,  1, 19,
                20, 20, 21, 22, 23,  1, 19, 19, 19, 19,
                24, 24, 25, 26, 27, 28, 29, 29, 19, 19,
                24, 30, 31, 30, 19, 34, 19, 19, 19, 19,
            },
            {
                24, 30, 30, 30, 19, 19, 19, 19, 38, 19,
                40, 41, 30, 42, 43, 43, 43, 19, 19, 44,
                40, 41, 41, 43, 43, 19, 19, 19, 19, 44,
                40, 40, 40, 19, 19, 19, 45, 45, 19, 44,
                46, 40, 19, 19, 47, 19, 19, 19, 44, 44,
            },
            {
                46, 46, 49, 19, 19, 19, 44, 44, 44, 44,
                49, 49, 49, 49, 19, 44, 44, 52, 52, 44,
                19, 19, 19, 19, 19, 44, 44, 44, 44, 44,
                19, 19, 44, 44, 44, 44, 53, 44, 44, 54,
                55, 55, 56, 56, 44, 57, 58, 59, 59, 60,
            },
        },
        {
            {
                 0,  0,  1,  2,  1,  1,  1,  1,  1,  3,
                 4,  0,  1,  1,  1,  1,  3,  1,  3,  3,
                 0,  0,  1,  3,  3,  3,  1,  3,  3,  1,
                 0,  1,  1,  1,  1,  1,  3,  3,  1,  1,
                 1,  0,  0,  1,  5,  1,  1,  1,  1,  1,
            },
            {
                 1,  0,  0,  1,  1,  5,  1,  0,  0,  1,
                 0,  0,  1,  0,  1,  1,  1,  1,  1,  0,
                 1,  1,  0,  1,  0,  1,  0,  0,  0,  0,
                 0,  0,  1,  0,  1,  0,  1,  1,  0,  0,
                 0,  1,  0,  1,  0,  1,  0,  0,  0,  0,
            },
            {
                 0,  1,  1,  1,  0,  0,  0,  0, 12,  0,
                 1,  0,  1,  0,  1,  1,  1,  0,  0, 12,
                 1,  0,  0,  1,  1,  0,  0,  0,  0, 12,
                 1,  1,  1,  0,  0,  0, 12, 12,  0, 12,
                 0,  1,  0,  0, 14,  0,  0,  0, 12, 12,
            },
            {
                 0,  0,  1,  0,  0,  0, 12, 12, 12, 12,
                 1,  1,  1,  1,  0, 12, 12, 19, 19, 12,
                 0,  0,  0,  0,  0, 12, 12, 12, 12, 12,
                 0,  0, 12, 12, 12, 12, 20, 12, 12, 20,
                12, 12,  0,  0, 12, 20, 12, 20, 20, 12,
            },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(use_case_09)
{
    // Single clump
    auto const x{lue::policy::no_data_value<std::int32_t>};

    test_clump<std::int32_t>(
        {10, 10},
        {5, 5},
        // clang-format off
        {
            {
                2, 2, 2, 2, 2,
                2, x, x, x, x,
                2, x, x, x, x,
                2, x, x, x, x,
                2, x, x, x, x,
            },
            {
                2, 2, 2, 2, 2,
                x, x, x, x, x,
                x, x, x, x, x,
                x, x, x, x, x,
                x, x, x, x, 2,
            },
            {
                2, x, x, x, x,
                2, x, x, x, x,
                2, x, x, x, x,
                2, x, x, x, x,
                2, 2, 2, 2, 2
            },
            {
                x, x, x, x, 2,
                x, x, x, x, 2,
                x, x, x, x, 2,
                x, x, x, x, 2,
                2, 2, 2, 2, 2
            },
        },
        {
            {
                0, 0, 0, 0, 0,
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
            },
            {
                0, 0, 0, 0, 0,
                x, x, x, x, x,
                x, x, x, x, x,
                x, x, x, x, x,
                x, x, x, x, 0,
            },
            {
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
                0, 0, 0, 0, 0
            },
            {
                x, x, x, x, 0,
                x, x, x, x, 0,
                x, x, x, x, 0,
                x, x, x, x, 0,
                0, 0, 0, 0, 0
            },
        },
        {
            {
                0, 0, 0, 0, 0,
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
            },
            {
                0, 0, 0, 0, 0,
                x, x, x, x, x,
                x, x, x, x, x,
                x, x, x, x, x,
                x, x, x, x, 0,
            },
            {
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
                0, x, x, x, x,
                0, 0, 0, 0, 0
            },
            {
                x, x, x, x, 0,
                x, x, x, x, 0,
                x, x, x, x, 0,
                x, x, x, x, 0,
                0, 0, 0, 0, 0
            },
        }  // clang-format on
    );
}


namespace lue::detail {

    template<typename T>
    class ArrayTraits<std::shared_ptr<std::vector<T>>>
    {

        public:

            using Element = T;
    };

}  // namespace lue::detail


BOOST_AUTO_TEST_CASE(random_input)
{
    // Random distribution of zone IDs and no-data values. Verify the algorithm finishes
    // without crashing. Verify the resulting number of clumps equals the result of passing in an array with
    // only a single partition.
    lue::Rank const rank{2};

    using ZoneElement = std::int32_t;
    using Shape = lue::Shape<lue::Count, rank>;
    using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;

    std::random_device random_device{};
    std::default_random_engine random_number_engine(random_device());

    // Shape const array_shape{10, 10};
    // Shape const partition_shape{5, 5};

    Shape const array_shape = [&]()
    {
        lue::Count const min{100};
        lue::Count const max{500};
        std::uniform_int_distribution<lue::Count> distribution(min, max);

        return Shape{
            distribution(random_number_engine),
            distribution(random_number_engine),
        };
    }();
    Shape const partition_shape = [&]()
    {
        lue::Count const min{40};
        lue::Count const max{50};
        std::uniform_int_distribution<lue::Count> distribution(min, max);

        return Shape{
            distribution(random_number_engine),
            distribution(random_number_engine),
        };
    }();
    lue::Count const nr_zones = [&]()
    {
        lue::Count const min{1};
        lue::Count const max{5};
        std::uniform_int_distribution<lue::Count> distribution(min, max);

        return distribution(random_number_engine);
    }();

    using Buffer = std::vector<ZoneElement>;
    using BufferHandle = std::shared_ptr<Buffer>;

    BufferHandle buffer_handle = std::make_shared<Buffer>(lue::nr_elements(array_shape));
    {
        std::uniform_int_distribution<ZoneElement> distribution(1, static_cast<ZoneElement>(nr_zones));
        auto const no_data{lue::policy::no_data_value<ZoneElement>};

        std::for_each(
            buffer_handle->begin(),
            buffer_handle->end(),
            [&random_number_engine, &distribution, no_data](ZoneElement& zone_id)
            {
                zone_id = distribution(random_number_engine);

                if (zone_id == 1)
                {
                    zone_id = no_data;
                }
            });
    }

    // Create a partitioned array, passing in the buffer
    using Functor = lue::InstantiateFromBuffer<BufferHandle, rank>;

    Functor functor{buffer_handle, [](BufferHandle const& handle) -> ZoneElement* { return handle->data(); }};

    // Create two arrays with the same values but different partitioning
    ZoneArray zone_array_single_partition = lue::create_partitioned_array(array_shape, array_shape, functor);
    ZoneArray zone_array_multiple_partitions =
        lue::create_partitioned_array(array_shape, partition_shape, functor);

    using namespace lue::value_policies;

    // lue::wait_all(zone_array.partitions());
    // hpx::cout << zone_array << std::endl;

    // Test whether clumping the different partitioned arrays result in the same number of clumps. If so,
    // merging clumps seems to have worked.
    BOOST_TEST_CONTEXT("nondiagonal")
    {
        auto const nr_clumps1 =
            unique(clump(zone_array_single_partition, lue::Connectivity::nondiagonal)).get().size();
        auto const nr_clumps2 =
            unique(clump(zone_array_multiple_partitions, lue::Connectivity::nondiagonal)).get().size();
        BOOST_CHECK_EQUAL(nr_clumps1, nr_clumps2);
    }

    BOOST_TEST_CONTEXT("diagonal")
    {
        auto const nr_clumps1 =
            unique(clump(zone_array_single_partition, lue::Connectivity::diagonal)).get().size();
        auto const nr_clumps2 =
            unique(clump(zone_array_multiple_partitions, lue::Connectivity::diagonal)).get().size();
        BOOST_CHECK_EQUAL(nr_clumps1, nr_clumps2);
    }
}
