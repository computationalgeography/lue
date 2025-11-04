#define BOOST_TEST_MODULE lue framework algorithm focal_mean
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_mean.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


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
    auto const kernel = lue::box_kernel<lue::BooleanElement, 2>(1, 1);
    auto const result_we_got = lue::value_policies::focal_mean(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<ElementArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                 0.3333, 0.6,   -5.4,  -4.83,   -7.75,
                 1.2,    1.375, -2.25, -2.2222, -4.17,
                 1.4,    1.625,  6.75,  5.89,    7.83,
                 0.667,  1.4444, 6.67,  7.22,    9.33,
                -0.25,   0.833,  8.5,   9.33,    12.5,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_close(result_we_got, result_we_want, Element{1e-3});
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
    auto const result_we_got = lue::value_policies::focal_mean(array, kernel);

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
