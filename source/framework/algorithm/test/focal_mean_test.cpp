#define BOOST_TEST_MODULE lue framework algorithm focal_mean
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_mean.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(focal_mean_2d_float64)
{
    using Element = double;
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
    auto focal_mean = lue::value_policies::focal_mean(array, kernel);

    Element const nd{lue::policy::no_data_value<Element>};

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape,
        partition_shape,
        {
            {nd, nd, nd, nd, 11.0, 12.0, nd, 20.0, 21.0},
            {nd, nd, nd, 13.0, 14.0, 15.0, 22.0, 23.0, 24.0},
            {nd, nd, nd, 16.0, 17.0, nd, 25.0, 26.0, nd},

            {nd, 29.0, 30.0, nd, 38.0, 39.0, nd, 47.0, 48.0},
            {31.0, 32.0, 33.0, 40.0, 41.0, 42.0, 49.0, 50.0, 51.0},
            {34.0, 35.0, nd, 43.0, 44.0, nd, 52.0, 53.0, nd},

            {nd, 56.0, 57.0, nd, 65.0, 66.0, nd, nd, nd},
            {58.0, 59.0, 60.0, 67.0, 68.0, 69.0, nd, nd, nd},
            {61.0, 62.0, nd, 70.0, 71.0, nd, nd, nd, nd},
        });

    lue::test::check_arrays_are_equal(focal_mean, array_we_want);
}
