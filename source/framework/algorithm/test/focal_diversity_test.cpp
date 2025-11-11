#define BOOST_TEST_MODULE lue framework algorithm focal_diversity
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/value_policies/focal_diversity.hpp"
#include "lue/framework/algorithm/value_policies/logical_and.hpp"
#include "lue/framework/algorithm/value_policies/not_equal_to.hpp"
#include "lue/framework/algorithm/value_policies/unique_id.hpp"
#include "lue/framework/algorithm/value_policies/where.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(pcraster_example)
{
    using Element = lue::LargestSignedIntegralElement;
    using Count = lue::CountElement;
    std::size_t const rank = 2;

    using ElementArray = lue::PartitionedArray<Element, rank>;
    using CountArray = lue::PartitionedArray<Count, rank>;
    using Shape = lue::ShapeT<ElementArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    Element const x{lue::policy::no_data_value<Element>};

    auto const array = lue::test::create_partitioned_array<ElementArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                1,  1, 1, -4, -4,
                2,  x, 2, -4, -4,
                3,  6, 3,  3,  3,
                4,  2, 3,  3,  3,
                0, 14, 3, -1,  0,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const kernel = lue::box_kernel<lue::BooleanElement, 2>(1, 1);
    auto const result_we_got = lue::value_policies::focal_diversity<Count>(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<CountArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                2, 2, 3, 3, 1,
                4, 4, 5, 4, 2,
                4, 4, 4, 3, 2,
                6, 6, 5, 3, 3,
                4, 5, 4, 3, 3,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(focal_diversity_2d_int32)
{
    using Element = lue::LargestSignedIntegralElement;
    using Count = lue::CountElement;
    std::size_t const rank = 2;

    using ElementArray = lue::PartitionedArray<Element, rank>;
    using CountArray = lue::PartitionedArray<Count, rank>;
    using Shape = lue::ShapeT<ElementArray>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    using namespace lue::value_policies;

    // [ 1  2  3  4  5  6  7  8  9]
    // [10 11 12 13 14 15 16 17 18]
    // [19 20 21 22 23 24 25 26 27]
    // [28 29 30 31 32 33 34 35 36]
    // [37 38 39 40 41 42 43 44 45]
    // [46 47 48 49 50 51 52 53 54]
    // [55 56 57 58 59 60 61 62 63]
    // [64 65 66 67 68 69 70 71 72]
    // [73 74 75 76 77 78 79 80 81]

    ElementArray array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};
    lue::range(array, Element{1}).get();

    auto const kernel = lue::box_kernel<lue::BooleanElement, rank>(1, 1);
    CountArray const result_we_got = focal_diversity<Count>(array, kernel);

    auto const result_we_want = lue::test::create_partitioned_array<CountArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                4, 6, 6,
                6, 9, 9,
                6, 9, 9
            },
            {
                6, 6, 6,
                9, 9, 9,
                9, 9, 9
            },
            {
                6, 6, 4,
                9, 9, 6,
                9, 9, 6
            },
            {
                6, 9, 9,
                6, 9, 9,
                6, 9, 9
            },
            {
                9, 9, 9,
                9, 9, 9,
                9, 9, 9
            },
            {
                9, 9, 6,
                9, 9, 6,
                9, 9, 6
            },
            {
                6, 9, 9,
                6, 9, 9,
                4, 6, 6
            },
            {
                9, 9, 9,
                9, 9, 9,
                6, 6, 6
            },
            {
                9, 9, 6,
                9, 9, 6,
                6, 6, 4
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(focal_diversity_no_data_focal_cell)
{
    using Element = lue::LargestSignedIntegralElement;
    using Count = lue::CountElement;
    std::size_t const rank = 2;

    using ElementArray = lue::PartitionedArray<Element, rank>;
    using BooleanArray = lue::PartitionedArray<lue::BooleanElement, rank>;
    using CountArray = lue::PartitionedArray<Count, rank>;
    using Shape = lue::ShapeT<ElementArray>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    using namespace lue::value_policies;

    // 1 2 3   3 5 3
    // 4 x 6 → 5 8 5
    // 7 8 9   3 5 3
    BooleanArray const condition{
        lue::create_partitioned_array<lue::BooleanElement>(array_shape, partition_shape, 1)};
    ElementArray array = unique_id<Element>(condition);
    array = where(array != Element{5}, array);

    auto const kernel = lue::box_kernel<lue::BooleanElement, rank>(1, 1);
    CountArray const result_we_got = focal_diversity<Count>(array, kernel);

    auto const result_we_want = lue::test::create_partitioned_array<CountArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                3, 5, 3,
                5, 8, 5,
                3, 5, 3,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(focal_diversity_no_data_neighbourhood_cell)
{
    using Element = lue::LargestSignedIntegralElement;
    using Count = lue::CountElement;
    std::size_t const rank = 2;

    using ElementArray = lue::PartitionedArray<Element, rank>;
    using BooleanArray = lue::PartitionedArray<lue::BooleanElement, rank>;
    using CountArray = lue::PartitionedArray<Count, rank>;
    using Shape = lue::ShapeT<ElementArray>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    using namespace lue::value_policies;

    // x 2 3   3 5 4
    // 4 5 6 → 4 7 5
    // 7 x 9   3 5 3
    BooleanArray const condition{
        lue::create_partitioned_array<lue::BooleanElement>(array_shape, partition_shape, 1)};
    ElementArray array = unique_id<Element>(condition);
    array = where(array != Element{1} && array != Element{8}, array);

    auto const kernel = lue::box_kernel<lue::BooleanElement, rank>(1, 1);
    CountArray const result_we_got = focal_diversity<Count>(array, kernel);

    auto const result_we_want = lue::test::create_partitioned_array<CountArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                3, 5, 4,
                4, 7, 5,
                3, 5, 3,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}
