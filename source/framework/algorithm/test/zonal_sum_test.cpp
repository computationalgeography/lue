#define BOOST_TEST_MODULE lue framework algorithm zonal_sum
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/zonal_sum.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(zonal_sum_2d_int32)
{
    using Value = std::int32_t;
    using Class = std::uint64_t;
    std::size_t const rank = 2;

    using ValueArray = lue::PartitionedArray<Value, rank>;
    using ClassArray = lue::PartitionedArray<Class, rank>;
    using Shape = lue::ShapeT<ValueArray>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    //  1  2  3 |  4  5  6 |  7  8  9
    // 10 11 12 | 13 14 15 | 16 17 18
    // 19 20 21 | 22 23 24 | 25 26 27
    // ---------+----------+---------
    // 28 29 30 | 31 32 33 | 34 35 36
    // 37 38 39 | 40 41 42 | 43 44 45
    // 46 47 48 | 49 50 51 | 52 53 54
    // ---------+----------+---------
    // 55 56 57 | 58 59 60 | 61 62 63
    // 64 65 66 | 67 68 69 | 70 71 72
    // 73 74 75 | 76 77 78 | 79 80 81
    ValueArray value_array{array_shape, partition_shape};
    lue::range(value_array, hpx::make_ready_future<Value>(1).share()).wait();

    ClassArray class_array = lue::array_partition_id(value_array);

    auto zonal_sum = lue::zonal_sum(value_array, class_array);

    ValueArray array_we_want = lue::test::create_partitioned_array<ValueArray>(
        array_shape, partition_shape, {
            {  99,  99,  99,  99,  99,  99,  99,  99,  99 },
            { 126, 126, 126, 126, 126, 126, 126, 126, 126 },
            { 153, 153, 153, 153, 153, 153, 153, 153, 153 },
            { 342, 342, 342, 342, 342, 342, 342, 342, 342 },
            { 369, 369, 369, 369, 369, 369, 369, 369, 369 },
            { 396, 396, 396, 396, 396, 396, 396, 396, 396 },
            { 585, 585, 585, 585, 585, 585, 585, 585, 585 },
            { 612, 612, 612, 612, 612, 612, 612, 612, 612 },
            { 639, 639, 639, 639, 639, 639, 639, 639, 639 },
        });

    lue::test::check_arrays_are_equal(zonal_sum, array_we_want);
}
