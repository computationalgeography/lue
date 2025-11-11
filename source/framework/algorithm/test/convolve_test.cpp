#define BOOST_TEST_MODULE lue framework algorithm convolve
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/value_policies/convolve.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


// TODO Test with larger kernel radius
// TODO Test with different float weights

// TODO Test empty input raster
//
// TODO First row in each partitions is fragile. Sometimes it works,
//      sometimes it doesn't...


BOOST_AUTO_TEST_CASE(pcraster_example)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using ElementArray = lue::PartitionedArray<Element, rank>;
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
                0, -1, 1, -30,  0,
                2,  x, 1,   2, -3,
                3,  2, 3,   4,  2,
                0,  0, 2,  40,  2,
                1, -2, 4,   7,  1,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const kernel = lue::box_kernel<Element, rank>(1, 1);
    auto const result_we_got = lue::value_policies::convolve(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<ElementArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                 1,  3, -27, -29, -31,
                 6, 11, -18, -20, -25,
                 7, 13,  54,  53,  47,
                 4, 13,  60,  65,  56,
                -1,  5,  51,  56,  50,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_close(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using ElementArray = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<ElementArray>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    Element const x{lue::policy::no_data_value<Element>};

    // x x x   x x x
    // x x x → x x x
    // x x x   x x x
    auto const array{lue::create_partitioned_array<Element>(array_shape, partition_shape, x)};
    auto const kernel = lue::box_kernel<Element, rank>(1, 1);
    auto const result_we_got = lue::value_policies::convolve(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<ElementArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                x, x, x,
                x, x, x,
                x, x, x,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(window_total_2d)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    Array const array{lue::create_partitioned_array(array_shape, partition_shape, Element{1})};
    auto const kernel = lue::box_kernel<Element, rank>(1, 1);
    auto const result_we_got = lue::value_policies::convolve(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                4., 6., 6.,
                6., 9., 9.,
                6., 9., 9.
            },
            {
                6., 6., 6.,
                9., 9., 9.,
                9., 9., 9.
            },
            {
                6., 6., 4.,
                9., 9., 6.,
                9., 9., 6.
            },
            {
                6., 9., 9.,
                6., 9., 9.,
                6., 9., 9.
            },
            {
                9., 9., 9.,
                9., 9., 9.,
                9., 9., 9.
            },
            {
                9., 9., 6.,
                9., 9., 6.,
                9., 9., 6.
            },
            {
                6., 9., 9.,
                6., 9., 9.,
                4., 6., 6.
            },
            {
                9., 9., 9.,
                9., 9., 9.,
                6., 6., 6.
            },
            {
                9., 9., 6.,
                9., 9., 6.,
                6., 6., 4.
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(window_total_2d_single_partition)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};
    Array const array{lue::create_partitioned_array(array_shape, partition_shape, Element{1})};
    auto const kernel = lue::box_kernel<Element, rank>(1, 1);

    // [4. 6. 4.]
    // [6. 9. 6.]
    // [4. 6. 4.]
    auto const result_we_got = lue::value_policies::convolve(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                4., 6., 4.,
                6., 9., 6.,
                4., 6., 4.
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(window_total_2d_single_row_of_partitions)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{3, 9}};
    Shape const partition_shape{{3, 3}};
    Array const array{lue::create_partitioned_array(array_shape, partition_shape, Element{1})};
    auto const kernel = lue::box_kernel<Element, rank>(1, true);
    auto const result_we_got = lue::value_policies::convolve(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                4., 6., 6.,
                6., 9., 9.,
                4., 6., 6.
            },
            {
                6., 6., 6.,
                9., 9., 9.,
                6., 6., 6.
            },
            {
                6., 6., 4.,
                9., 9., 6.,
                6., 6., 4.
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(window_total_2d_single_col_of_partitions)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{9, 3}};
    Shape const partition_shape{{3, 3}};

    Array const array{lue::create_partitioned_array(array_shape, partition_shape, Element{1})};
    auto const kernel = lue::box_kernel<Element, rank>(1, 1);
    auto const result_we_got = lue::value_policies::convolve(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                4., 6., 4.,
                6., 9., 6.,
                6., 9., 6.
            },
            {
                6., 9., 6.,
                6., 9., 6.,
                6., 9., 6.
            },
            {
                6., 9., 6.,
                6., 9., 6.,
                4., 6., 4.
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}
