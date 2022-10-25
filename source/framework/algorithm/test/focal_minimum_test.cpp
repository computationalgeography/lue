#define BOOST_TEST_MODULE lue framework algorithm focal_minimum
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_minimum.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(focal_minimum_2d_int32)
{
    using Element = std::int32_t;
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

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<std::uint8_t, rank>(1, 1);
    auto focal_minimum = lue::value_policies::focal_minimum(array, kernel);

    Element const nd{lue::policy::no_data_value<Element>};

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape, partition_shape, {
            { nd, nd, nd, nd,  1,  2, nd, 10, 11 },
            { nd, nd, nd,  3,  4,  5, 12, 13, 14 },
            { nd, nd, nd,  6,  7, nd, 15, 16, nd },

            { nd, 19, 20, nd, 28, 29, nd, 37, 38 },
            { 21, 22, 23, 30, 31, 32, 39, 40, 41 },
            { 24, 25, nd, 33, 34, nd, 42, 43, nd },

            { nd, 46, 47, nd, 55, 56, nd, nd, nd },
            { 48, 49, 50, 57, 58, 59, nd, nd, nd },
            { 51, 52, nd, 60, 61, nd, nd, nd, nd },
        });

    lue::test::check_arrays_are_equal(focal_minimum, array_we_want);
}
