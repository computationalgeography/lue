#define BOOST_TEST_MODULE lue framework algorithm clump
#include "lue/framework/algorithm/value_policies/clump.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <random>

#include <hpx/iostream.hpp>


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
void test_clump_single_partition(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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

    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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

    test_clump_single_partition<std::int32_t>(
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

    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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

    test_clump_single_partition<std::int32_t>(
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

    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
              2, 0, 0, },
            { 0, 0, 3,
              0, 0, 0,
              0, 0, 0, },
            { 4, 0, 0,
              0, 0, 0,
              0, 0, 0, },
        },
        {
            { 0, 0, 0,
              0, 0, 0,
              0, 0, 1, },
            { 0, 0, 0,
              0, 0, 0,
              2, 0, 0, },
            { 0, 0, 2,
              0, 0, 0,
              0, 0, 0, },
            { 1, 0, 0,
              0, 0, 0,
              0, 0, 0, },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(reclass)
{
    test_clump_single_partition<std::int32_t>(
        {6, 6},
        {3, 3},
        // clang-format off
        {
            {
                0, 0, 1,
                0, 0, 1,
                3, 3, 4,
            },


            {
                1, 0, 0,
                1, 0, 0,
                5, 6, 6,
            },
            {
                3, 3, 5,
                0, 0, 8,
                0, 0, 8,
            },
            {
                4, 6, 6,
                8, 0, 0,
                8, 0, 0,
            },
        },
        {
            {
                0, 0, 1,
                0, 0, 1,
                2, 2, 3,
            },


            {
                1, 4, 4,
                1, 4, 4,
                5, 6, 6,
            },
            {
                2, 2, 7,
                8, 8, 9,
                8, 8, 9,
            },
            {
                10,  6,  6,
                 9, 11, 11,
                 9, 11, 11,
            },
        },
        {
            {
                0, 0, 1,
                0, 0, 1,
                2, 2, 3,
            },


            {
                1, 4, 4,
                1, 4, 4,
                5, 6, 6,
            },
            {
                2, 2, 5,
                7, 7, 8,
                7, 7, 8,
            },
            {
                3, 6, 6,
                8, 9, 9,
                8, 9, 9,
            },
        }  // clang-format on
    );
}


BOOST_AUTO_TEST_CASE(u_turn_1)
{
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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
    test_clump_single_partition<std::int32_t>(
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


// BOOST_AUTO_TEST_CASE(random_input)
// {
//     // Random distribution of zone IDs and no-data values. Verify the algorithm finishes
//     // without crashing.
//     lue::Rank const rank{2};
//
//     using ZoneElement = std::int32_t;
//     using ClumpElement = ZoneElement;
//     using Shape = lue::Shape<lue::Count, rank>;
//     using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;
//     using ClumpArray = lue::PartitionedArray<ClumpElement, rank>;
//
//     std::random_device random_device{};
//     std::default_random_engine random_number_engine(random_device());
//
//     Shape const array_shape = [&]()
//     {
//         lue::Count const min{100};
//         lue::Count const max{500};
//         std::uniform_int_distribution<lue::Count> distribution(min, max);
//
//         return Shape{
//             distribution(random_number_engine),
//             distribution(random_number_engine),
//         };
//     }();
//     Shape const partition_shape = [&]()
//     {
//         lue::Count const min{40};
//         lue::Count const max{50};
//         std::uniform_int_distribution<lue::Count> distribution(min, max);
//
//         return Shape{
//             distribution(random_number_engine),
//             distribution(random_number_engine),
//         };
//     }();
//     lue::Count const nr_zones = [&]()
//     {
//         lue::Count const min{1};
//         lue::Count const max{10};
//         std::uniform_int_distribution<lue::Count> distribution(min, max);
//
//         return distribution(random_number_engine);
//     }();
//
//     using namespace lue::value_policies;
//
//     ZoneArray const zone_array{
//         lue::value_policies::uniform<ZoneElement>(array_shape, partition_shape, 1, nr_zones)};
//
//     hpx::cout << "\n\n---------------- (" << array_shape[0] << ", " << array_shape[1] << ")\n";
//
//     BOOST_TEST_CONTEXT("nondiagonal")
//     {
//         ClumpArray const result_we_got = lue::value_policies::clump(zone_array,
//         lue::Connectivity::nondiagonal); auto const& partitions = result_we_got.partitions();
//         hpx::wait_all(partitions.begin(), partitions.end());
//     }
//
//     BOOST_TEST_CONTEXT("diagonal")
//     {
//         ClumpArray const result_we_got = lue::value_policies::clump(zone_array,
//         lue::Connectivity::diagonal); auto const& partitions = result_we_got.partitions();
//         hpx::wait_all(partitions.begin(), partitions.end());
//     }
//     hpx::cout << "/----------------\n\n";
// }
