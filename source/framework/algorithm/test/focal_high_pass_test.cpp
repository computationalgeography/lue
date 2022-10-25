#define BOOST_TEST_MODULE lue framework algorithm focal_high_pass
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_high_pass.hpp"
#include "lue/framework/algorithm/range.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(focal_high_pass_2d_float64)
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
    auto focal_high_pass = lue::value_policies::focal_high_pass(array, kernel);

    Element const nd{lue::policy::no_data_value<Element>};

    Array array_we_want = lue::test::create_partitioned_array<Array>(
        array_shape, partition_shape, {
            {    nd,    nd,    nd,    nd,  88.0,  96.0,    nd, 160.0, 168.0 },
            {    nd,    nd,    nd, 104.0, 112.0, 120.0, 176.0, 184.0, 192.0 },
            {    nd,    nd,    nd, 128.0, 136.0,    nd, 200.0, 208.0,    nd },

            {    nd, 232.0, 240.0,    nd, 304.0, 312.0,    nd, 376.0, 384.0 },
            { 248.0, 256.0, 264.0, 320.0, 328.0, 336.0, 392.0, 400.0, 408.0 },
            { 272.0, 280.0,    nd, 344.0, 352.0,    nd, 416.0, 424.0,    nd },

            {    nd, 448.0, 456.0,    nd, 520.0, 528.0,    nd,    nd,    nd },
            { 464.0, 472.0, 480.0, 536.0, 544.0, 552.0,    nd,    nd,    nd },
            { 488.0, 496.0,    nd, 560.0, 568.0,    nd,    nd,    nd,    nd },
        });

    lue::test::check_arrays_are_equal(focal_high_pass, array_we_want);
}
