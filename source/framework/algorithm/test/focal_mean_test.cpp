#define BOOST_TEST_MODULE lue framework algorithm focal_mean
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/focal_mean.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
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
    Array array{array_shape, partition_shape};
    lue::range(array, hpx::make_ready_future<Element>(1).share()).wait();

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<bool, rank>(1, true);
    auto focal_mean = lue::focal_mean(array, kernel);

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape, partition_shape, {
            {  6.0 ,  6.5,  7.5,  10.5, 11.0, 12.0, 19.5, 20.0, 21.0 },
            {  8.5 ,  9.5, 10.5,  13.0, 14.0, 15.0, 22.0, 23.0, 24.0 },
            { 11.5 , 12.5, 13.0,  16.0, 17.0, 17.5, 25.0, 26.0, 26.5 },
            { 28.5 , 29.0, 30.0,  37.5, 38.0, 39.0, 46.5, 47.0, 48.0 },
            { 31.0 , 32.0, 33.0,  40.0, 41.0, 42.0, 49.0, 50.0, 51.0 },
            { 34.0 , 35.0, 35.5,  43.0, 44.0, 44.5, 52.0, 53.0, 53.5 },
            { 55.5 , 56.0, 57.0,  64.5, 65.0, 66.0, 69.0, 69.5, 70.5 },
            { 58.0 , 59.0, 60.0,  67.0, 68.0, 69.0, 71.5, 72.5, 73.5 },
            { 61.0 , 62.0, 62.5,  70.0, 71.0, 71.5, 74.5, 75.5, 76.0 },
        });

    lue::test::check_equal_array(focal_mean, array_we_want);
}
