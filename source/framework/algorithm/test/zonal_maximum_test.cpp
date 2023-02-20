#define BOOST_TEST_MODULE lue framework algorithm zonal_maximum
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/zonal_maximum.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Value = std::uint64_t;
    using Class = std::uint64_t;
    std::size_t const rank = 2;

    using ValueArray = lue::PartitionedArray<Value, rank>;
    using ClassArray = lue::PartitionedArray<Class, rank>;
    using Shape = lue::ShapeT<ValueArray>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    ValueArray value_array{
        lue::array_partition_id(lue::create_partitioned_array<Class>(array_shape, partition_shape))};

    ClassArray class_array{
        lue::array_partition_id(lue::create_partitioned_array<Class>(array_shape, partition_shape))};

    auto zonal_maximum = lue::value_policies::zonal_maximum<Value>(value_array, class_array);

    ValueArray array_we_want = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {1, 1, 1, 1, 1, 1, 1, 1, 1},
            {2, 2, 2, 2, 2, 2, 2, 2, 2},
            {3, 3, 3, 3, 3, 3, 3, 3, 3},
            {4, 4, 4, 4, 4, 4, 4, 4, 4},
            {5, 5, 5, 5, 5, 5, 5, 5, 5},
            {6, 6, 6, 6, 6, 6, 6, 6, 6},
            {7, 7, 7, 7, 7, 7, 7, 7, 7},
            {8, 8, 8, 8, 8, 8, 8, 8, 8},
        });

    lue::test::check_arrays_are_equal(zonal_maximum, array_we_want);
}


BOOST_AUTO_TEST_CASE(use_case_02)
{
    using Value = std::int32_t;
    using Class = std::uint64_t;
    std::size_t const rank = 2;

    using ValueArray = lue::PartitionedArray<Value, rank>;
    using ClassArray = lue::PartitionedArray<Class, rank>;
    using Shape = lue::ShapeT<ValueArray>;

    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    // Class const cx{lue::policy::no_data_value<Class>};  // Class no-data
    Value const vx{lue::policy::no_data_value<Value>};  // Value no-data

    //  1  2  3 |  4  5  6 |  7  8  9
    // 10 11 12 | 13 14 15 | 16 17 18
    // 19 20 21 | 22 23 24 | 25 26 27
    // ---------+----------+---------
    // 28 29 30 | 31 32 33 | 34 35 36
    // 37 38 39 | 40 41 42 | 43 44 45
    // 46 47 48 | 49 50 vx | 52 53 54
    // ---------+----------+---------
    // vx vx vx | 58 59 60 | 61 62 63
    // vx vx vx | 67 68 69 | 70 71 72
    // vx vx vx | 76 77 78 | 79 80 81

    ValueArray value_array = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {
            {1, 2, 3, 10, 11, 12, 19, 20, 21},
            {4, 5, 6, 13, 14, 15, 22, 23, 24},
            {7, 8, 9, 16, 17, 18, 25, 26, 27},
            {28, 29, 30, 37, 38, 39, 46, 47, 48},
            {31, 32, 33, 40, 41, 42, 49, 50, vx},
            {34, 35, 36, 43, 44, 45, 52, 53, 54},
            {vx, vx, vx, vx, vx, vx, vx, vx, vx},
            {58, 59, 60, 67, 68, 69, 76, 77, 78},
            {61, 62, 63, 70, 71, 72, 79, 80, 81},
        });

    ClassArray class_array = lue::array_partition_id(value_array);

    auto zonal_maximum = lue::value_policies::zonal_maximum(value_array, class_array);

    ValueArray array_we_want = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {
            {21, 21, 21, 21, 21, 21, 21, 21, 21},
            {24, 24, 24, 24, 24, 24, 24, 24, 24},
            {27, 27, 27, 27, 27, 27, 27, 27, 27},
            {48, 48, 48, 48, 48, 48, 48, 48, 48},
            {50, 50, 50, 50, 50, 50, 50, 50, 50},
            {54, 54, 54, 54, 54, 54, 54, 54, 54},
            {vx, vx, vx, vx, vx, vx, vx, vx, vx},
            {78, 78, 78, 78, 78, 78, 78, 78, 78},
            {81, 81, 81, 81, 81, 81, 81, 81, 81},
        });

    lue::test::check_arrays_are_equal(zonal_maximum, array_we_want);
}
