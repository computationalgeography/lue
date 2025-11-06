#define BOOST_TEST_MODULE lue framework algorithm focal_majority
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_majority.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(inspired_by_pcraster_example)
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
                17, x,  4,  x,  3,
                17, x,  2,  2,  3,
                17, 8, -7,  6,  4,
                 0, 0, 15, 15, 15,
                 2, 2, 15, 15, 15,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const kernel = lue::box_kernel<lue::BooleanElement, 2>(1, 1);
    auto const result_we_got = lue::value_policies::focal_majority(array, kernel);
    auto const result_we_want = lue::test::create_partitioned_array<ElementArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                17, 17,  2,  2,  3,
                17, 17,  2,  2,  3,
                 0,  0,  2, 15, 15,
                 0,  0, 15, 15, 15,
                 0,  0, 15, 15, 15,
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
    auto const result_we_got = lue::value_policies::focal_majority(array, kernel);

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
