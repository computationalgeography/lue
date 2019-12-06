#define BOOST_TEST_MODULE lue framework algorithm focal_sum
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/focal_sum.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(focal_sum_2d_int32)
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
    Array array{array_shape, partition_shape};
    lue::range(array, hpx::make_ready_future<Element>(1).share()).wait();

    // [true true true]
    // [true true true]
    // [true true true]
    auto const kernel = lue::box_kernel<bool, rank>(1, true);
    auto focal_sum = lue::focal_sum(array, kernel);

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape, partition_shape, {
            {  24,  39,  45,  63,  99, 108, 117, 180, 189 },
            {  51,  57,  63, 117, 126, 135, 198, 207, 216 },
            {  69,  75,  52, 144, 153, 105, 225, 234, 159 },
            { 171, 261, 270, 225, 342, 351, 279, 423, 432 },
            { 279, 288, 297, 360, 369, 378, 441, 450, 459 },
            { 306, 315, 213, 387, 396, 267, 468, 477, 321 },
            { 333, 504, 513, 387, 585, 594, 276, 417, 423 },
            { 522, 531, 540, 603, 612, 621, 429, 435, 441 },
            { 549, 558, 375, 630, 639, 429, 447, 453, 304 },
        });

    lue::test::check_equal_array(focal_sum, array_we_want);
}
