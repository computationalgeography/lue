#define BOOST_TEST_MODULE lue framework algorithm range
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <fmt/format.h>


BOOST_AUTO_TEST_CASE(range_2d_int32)
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
    lue::range(array, hpx::make_ready_future<Element>(1).share()).wait();

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape, partition_shape, {
            {  1,  2,  3, 10, 11, 12, 19, 20, 21 },
            {  4,  5,  6, 13, 14, 15, 22, 23, 24 },
            {  7,  8,  9, 16, 17, 18, 25, 26, 27 },
            { 28, 29, 30, 37, 38, 39, 46, 47, 48 },
            { 31, 32, 33, 40, 41, 42, 49, 50, 51 },
            { 34, 35, 36, 43, 44, 45, 52, 53, 54 },
            { 55, 56, 57, 64, 65, 66, 73, 74, 75 },
            { 58, 59, 60, 67, 68, 69, 76, 77, 78 },
            { 61, 62, 63, 70, 71, 72, 79, 80, 81 },
        });

    lue::test::check_arrays_are_equal(array, array_we_want);
}
