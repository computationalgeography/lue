#define BOOST_TEST_MODULE lue framework algorithm focal_high_pass
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/value_policies/focal_high_pass.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(pcraster_example)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    Element const x{lue::policy::no_data_value<Element>};

    auto const array = lue::test::create_partitioned_array<Array>(
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
    auto const kernel = lue::box_kernel<lue::BooleanElement, 2>(1, 1);
    auto const result_we_got = lue::value_policies::focal_high_pass(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                -1, -12,  36, -301,  31,
                12,   x,  33,   54,  -8,
                20,  17,  -9,   15, -25,
                -4, -13, -26,  615, -34,
                 8, -27,  -7,   21, -43,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(focal_high_pass_2d_float)
{
    using Element = lue::FloatingPointElement<0>;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    // [ 1  2  3  4  5  6  7  8  9]
    // [10 11 12 13 14 15 16 17 18]
    // [19 20 21 22 23 24 25 26 27]
    // [28 29 30 31 32 33 34 35 36]
    // [37 38 39 40 41 42 43 44 45]
    // [46 47 48 49 50 51 52 53 54]
    // [55 56 57 58 59 60 61 62 63]
    // [64 65 66 67 68 69 70 71 72]
    // [73 74 75 76 77 78 79 80 81]
    Array array{lue::create_partitioned_array<Element>(array_shape, partition_shape)};
    lue::range(array, Element{1}).get();

    auto const kernel = lue::box_kernel<lue::BooleanElement, 2>(1, 1);
    auto focal_high_pass = lue::value_policies::focal_high_pass(array, kernel);

    auto array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                -17.0, -17.0, -12.0,
                 47.0,  88.0,  96.0,
                 92.0, 160.0, 168.0,
            },
            {
                 -7.0,  -2.0,   3.0,
                104.0, 112.0, 120.0,
                176.0, 184.0, 192.0,
            },
            {
                  8.0,  13.0,  11.0,
                128.0, 136.0,  93.0,
                200.0, 208.0, 138.0,
            },
            {
                137.0, 232.0, 240.0,
                182.0, 304.0, 312.0,
                227.0, 376.0, 384.0,
            },
            {
                248.0, 256.0, 264.0,
                320.0, 328.0, 336.0,
                392.0, 400.0, 408.0,
            },
            {
                272.0, 280.0, 183.0,
                344.0, 352.0, 228.0,
                416.0, 424.0, 273.0,
            },
            {
                272.0, 448.0, 456.0,
                317.0, 520.0, 528.0,
                235.0, 397.0, 402.0,
            },
            {
                464.0, 472.0, 480.0,
                536.0, 544.0, 552.0,
                407.0, 412.0, 417.0,
            },
            {
                488.0, 496.0, 318.0,
                560.0, 568.0, 363.0,
                422.0, 427.0, 263.0,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(focal_high_pass, array_we_want);
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
    auto const kernel = lue::box_kernel<lue::BooleanElement, rank>(1, 1);
    auto const result_we_got = lue::value_policies::focal_high_pass(array, kernel);

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
