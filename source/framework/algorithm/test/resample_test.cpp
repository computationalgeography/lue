#define BOOST_TEST_MODULE lue framework algorithm resample
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/detail/resample.hpp"
#include "lue/framework/algorithm/resample.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/equal_to.hpp"
#include "lue/framework/algorithm/value_policies/resample.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"
#include <limits>
#include <random>

// #include "lue/framework/algorithm/array_partition_id.hpp"
// #include "lue/framework/io/gdal.hpp"


void test_partition_idx_mapping(
    lue::Shape<lue::Count, 2> const& target_shape_in_partitions,
    std::initializer_list<lue::Index> const& target_partition_idxs,
    lue::Shape<lue::Count, 2> const& source_shape_in_partitions,
    std::initializer_list<lue::Index> const& source_partition_idxs,
    lue::Count const count)
{
    assert(std::size(source_partition_idxs) == std::size(target_partition_idxs));

    for (std::size_t idx = 0; idx < std::size(source_partition_idxs); ++idx)
    {
        BOOST_REQUIRE_EQUAL(
            lue::detail::target_idx_to_source_idx(
                target_shape_in_partitions,
                std::data(target_partition_idxs)[idx],
                source_shape_in_partitions,
                count),
            std::data(source_partition_idxs)[idx]);
    }
}


BOOST_AUTO_TEST_CASE(target_partition_idx_to_source_partition_idx)
{
    // clang-format off
    test_partition_idx_mapping(
        {4, 4},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        {2, 2},
        {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3},
        2);

    test_partition_idx_mapping(
        {3, 9},
        {0, 3, 5, 20, 23, 24, 26},
        {1, 3},
        {0, 1, 1, 0, 1, 2, 2},
        3);

    test_partition_idx_mapping(
        {2, 5},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        {1, 2},
        {0, 0, 1, 1, 1, 0, 0, 1, 1, 1},
        2);
    // clang-format on
}


template<typename Element>
void test_downscale(
    lue::Shape<lue::Count, 2> const& source_array_shape,
    lue::Shape<lue::Count, 2> const& partition_shape,
    std::initializer_list<std::initializer_list<Element>> const& source_elements,
    lue::Count const count,
    lue::DownscaleStrategy const strategy,
    lue::Shape<lue::Count, 2> const& target_array_shape,
    std::initializer_list<std::initializer_list<Element>> const& target_elements)
{
    auto const source_array = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        source_array_shape, partition_shape, source_elements);
    auto const result_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        target_array_shape, partition_shape, target_elements);
    auto const result_we_got = lue::value_policies::downscale(source_array, count, strategy);
    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(downscale_assign_same_resolution)
{
    using Element = lue::SignedIntegralElement<0>;

    lue::Count const count{1};

    // clang-format off
    test_downscale<Element>(
        {3, 3},
        {3, 3},
        {{
            5, 5, 5,
            5, 5, 5,
            5, 5, 5,
        }},
        count,
        lue::DownscaleStrategy::assign,
        {3, 3},
        {{
            5, 5, 5,
            5, 5, 5,
            5, 5, 5,
        }}
    );
    // clang-format on
}


BOOST_AUTO_TEST_CASE(downscale_assign_signed_integral_2)
{
    using Element = lue::LargestSignedIntegralElement;

    lue::Count const count{2};

    // clang-format off
    test_downscale<Element>(
        {3, 9},
        {3, 3},
        {
            {
                 1,  2,  3,
                10, 11, 12,
                19, 20, 21,
            },
            {
                 4,  5,  6,
                13, 14, 15,
                22, 23, 24,
            },
            {
                 7,  8,  9,
                16, 17, 18,
                25, 26, 27,
            }
        },
        count,
        lue::DownscaleStrategy::assign,
        {6, 18},
        {
            {
                  1,  1,  2,
                  1,  1,  2,
                 10, 10, 11,
            },
            {
                  2,  3,  3,
                  2,  3,  3,
                 11, 12, 12,
            },
            {
                  4,  4,  5,
                  4,  4,  5,
                 13, 13, 14,
            },
            {
                  5,  6,  6,
                  5,  6,  6,
                 14, 15, 15,
            },
            {
                  7,  7,  8,
                  7,  7,  8,
                 16, 16, 17,
            },
            {
                  8,  9,  9,
                  8,  9,  9,
                 17, 18, 18,
            },
            {
                 10, 10, 11,
                 19, 19, 20,
                 19, 19, 20,
            },
            {
                 11, 12, 12,
                 20, 21, 21,
                 20, 21, 21,
            },
            {
                 13, 13, 14,
                 22, 22, 23,
                 22, 22, 23,
            },
            {
                 14, 15, 15,
                 23, 24, 24,
                 23, 24, 24,
            },
            {
                 16, 16, 17,
                 25, 25, 26,
                 25, 25, 26,
            },
            {
                 17, 18, 18,
                 26, 27, 27,
                 26, 27, 27,
            },
        }
    );
    // clang-format on
}


BOOST_AUTO_TEST_CASE(downscale_assign_signed_integral_2_merged_column)
{
    // Verify the larger partitions in the last column get handled correctly
    using Element = lue::LargestSignedIntegralElement;

    lue::Count const count{2};

    // clang-format off
    test_downscale<Element>(
        {3, 8},
        {3, 3},
        {
            {
                 1,  2,  3,
                 9, 10, 11,
                17, 18, 19,
            },
            {
                 4,  5,  6,  7,  8,
                12, 13, 14, 15, 16,
                20, 21, 22, 23, 24,
            },
        },
        count,
        lue::DownscaleStrategy::assign,
        {6, 16},
        {
            {
                 1,  1,  2,
                 1,  1,  2,
                 9,  9, 10,
            },
            {
                 2,  3,  3,
                 2,  3,  3,
                10, 11, 11,
            },
            {
                 4,  4,  5,
                 4,  4,  5,
                12, 12, 13,
            },
            {
                 5,  6,  6,
                 5,  6,  6,
                13, 14, 14,
            },
            {
                 7,  7,  8,  8,
                 7,  7,  8,  8,
                15, 15, 16, 16,
            },
            {
                 9,  9, 10,
                17, 17, 18,
                17, 17, 18,
            },
            {
                10, 11, 11,
                18, 19, 19,
                18, 19, 19,
            },
            {
                12, 12, 13,
                20, 20, 21,
                20, 20, 21,
            },
            {
                13, 14, 14,
                21, 22, 22,
                21, 22, 22,
            },
            {
                15, 15, 16, 16,
                23, 23, 24, 24,
                23, 23, 24, 24,
            },
        }
    );
    // clang-format on
}


BOOST_AUTO_TEST_CASE(downscale_assign_signed_integral_2_merged_row)
{
    // Verify the larger partitions in the last row get handled correctly
    using Element = lue::LargestSignedIntegralElement;

    lue::Count const count{2};

    // clang-format off
    test_downscale<Element>(
        {8, 3},
        {3, 3},
        {
            {
                 1,  2,  3,
                 4,  5,  6,
                 7,  8,  9,
            },
            {
                10, 11, 12,
                13, 14, 15,
                16, 17, 18,
                19, 20, 21,
                22, 23, 24,
            },
        },
        count,
        lue::DownscaleStrategy::assign,
        {16, 6},
        {
            {
                 1,  1,  2,
                 1,  1,  2,
                 4,  4,  5,
            },
            {
                 2,  3,  3,
                 2,  3,  3,
                 5,  6,  6,
            },
            {
                 4,  4,  5,
                 7,  7,  8,
                 7,  7,  8,
            },
            {
                 5,  6,  6,
                 8,  9,  9,
                 8,  9,  9,
            },
            {
                10, 10, 11,
                10, 10, 11,
                13, 13, 14,
            },
            {
                11, 12, 12,
                11, 12, 12,
                14, 15, 15,
            },
            {
                13, 13, 14,
                16, 16, 17,
                16, 16, 17,
            },
            {
                14, 15, 15,
                17, 18, 18,
                17, 18, 18,
            },
            {
                19, 19, 20,
                19, 19, 20,
                22, 22, 23,
                22, 22, 23,
            },
            {
                20, 21, 21,
                20, 21, 21,
                23, 24, 24,
                23, 24, 24,
            },
        }
    );
    // clang-format on
}


BOOST_AUTO_TEST_CASE(downscale_assign_signed_integral_2_merged_row_col)
{
    // Verify the larger partitions in the last row and column get handled correctly
    using namespace lue::value_policies;

    lue::Rank const rank{2};

    using Element = lue::LargestSignedIntegralElement;
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const source_array_shape{8, 8};
    Shape const partition_shape{3, 3};
    lue::Count const count{2};
    Element const fill_value = 5;

    Array const source_array = lue::create_partitioned_array(source_array_shape, partition_shape, fill_value);
    Array const target_array = downscale(source_array, count, lue::DownscaleStrategy::assign);
    BOOST_CHECK(all(target_array == fill_value).future().get());
}


BOOST_AUTO_TEST_CASE(downscale_assign_failing_case01)
{
    // Verify the larger partitions in the last row and column get handled correctly
    using namespace lue::value_policies;

    lue::Rank const rank{2};

    using Element = lue::LargestSignedIntegralElement;
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    {
        Shape const source_array_shape{1, 23};
        Shape const partition_shape{1, 4};
        lue::Count const count{2};
        Element const fill_value = 5;

        Array const source_array =
            lue::create_partitioned_array(source_array_shape, partition_shape, fill_value);
        Array const target_array = downscale(source_array, count, lue::DownscaleStrategy::assign);

        BOOST_CHECK(all(target_array == fill_value).future().get());
    }

    {
        Shape const source_array_shape{45, 1};
        Shape const partition_shape{4, 1};
        lue::Count const count{2};
        Element const fill_value = 5;

        Array const source_array =
            lue::create_partitioned_array(source_array_shape, partition_shape, fill_value);
        Array const target_array = downscale(source_array, count, lue::DownscaleStrategy::assign);

        BOOST_CHECK(all(target_array == fill_value).future().get());
    }

    {
        Shape const source_array_shape{45, 23};
        Shape const partition_shape{4, 4};
        lue::Count const count{2};
        Element const fill_value = 5;

        Array const source_array =
            lue::create_partitioned_array(source_array_shape, partition_shape, fill_value);
        Array const target_array = downscale(source_array, count, lue::DownscaleStrategy::assign);

        BOOST_CHECK(all(target_array == fill_value).future().get());
    }
}


BOOST_AUTO_TEST_CASE(assign_random_input)
{
    // Create random input array and check whether resample works
    lue::Rank const rank{2};
    using Element = lue::LargestSignedIntegralElement;
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    std::random_device random_device{};
    std::default_random_engine random_number_engine(random_device());

    auto generate_array_shape = [&]() -> Shape
    {
        lue::Count const min{100};
        lue::Count const max{500};
        std::uniform_int_distribution<lue::Count> distribution(min, max);

        return Shape{
            distribution(random_number_engine),
            distribution(random_number_engine),
        };
    };
    auto generate_partition_shape = [&]() -> Shape
    {
        lue::Count const min{40};
        lue::Count const max{50};
        std::uniform_int_distribution<lue::Count> distribution(min, max);

        return Shape{
            distribution(random_number_engine),
            distribution(random_number_engine),
        };
    };
    auto generate_count = [&]() -> lue::Count
    {
        lue::Count const min{1};
        lue::Count const max{10};
        std::uniform_int_distribution<lue::Count> distribution(min, max);

        return distribution(random_number_engine);
    };
    auto generate_fill_value = [&]() -> Element
    {
        Element const min{std::numeric_limits<Element>::min()};
        Element const max{std::numeric_limits<Element>::max()};
        std::uniform_int_distribution<Element> distribution(min, max);

        return distribution(random_number_engine);
    };

    using namespace lue::value_policies;

    std::size_t const nr_iterations = 1;

    for (std::size_t i = 0; i < nr_iterations; ++i)
    {
        Shape const source_array_shape = generate_array_shape();
        Shape const partition_shape = generate_partition_shape();
        Element const fill_value = generate_fill_value();
        lue::Count const count = generate_count();

        Array const source_array =
            lue::create_partitioned_array(source_array_shape, partition_shape, fill_value);
        Array const target_array = downscale(source_array, count, lue::DownscaleStrategy::assign);
        // lue::to_gdal(target_array, std::format("resample-{}.tif", i));
        // lue::to_gdal(lue::array_partition_id<std::int32_t>(target_array),
        // std::format("resample-{}-partition_id.tif", i));

        BOOST_TEST_CONTEXT(
            std::format(
                "i: {}\n"
                "    - source_array_shape: {}, {}\n"
                "    - fill_value        : {}\n"
                "    - partition_shape   : {}, {}\n"
                "    - count             : {}\n",
                i,
                std::get<0>(source_array_shape),
                std::get<1>(source_array_shape),
                fill_value,
                std::get<0>(partition_shape),
                std::get<1>(partition_shape),
                //
                count))
        {
            BOOST_CHECK(all(target_array == fill_value).future().get());
        }
    }
}
