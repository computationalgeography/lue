#define BOOST_TEST_MODULE lue framework algorithm focal_sum
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_sum.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(pcraster_example)
{
    using Element = lue::LargestSignedIntegralElement;
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
    auto const result_we_got = lue::value_policies::focal_sum(array, kernel);
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

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    using Element = lue::LargestSignedIntegralElement;
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
    auto const result_we_got = lue::value_policies::focal_sum(array, kernel);

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
