#define BOOST_TEST_MODULE lue framework algorithm focal_max
#include "lue/framework/core/component/partitioned_array.hpp"
// #include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/focal_max.hpp"
#include "lue/framework/algorithm/range.hpp"
// #include "lue/framework/algorithm/serialize/kernel.hpp"
// #include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
// #include "lue/framework/test/stream.hpp"


BOOST_AUTO_TEST_CASE(2d_int32)
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

    // +---+---+---+
    // | 1 | 1 | 1 |
    // +---+---+---+
    // | 1 | 1 | 1 |
    // +---+---+---+
    // | 1 | 1 | 1 |
    // +---+---+---+
    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    // [11 12 13 14 15 16 17 18 18]
    // [20 21 22 23 24 25 26 27 27]
    // [29 30 31 32 33 34 35 36 36]
    // [38 39 40 41 42 43 44 45 45]
    // [47 48 49 50 51 52 53 54 54]
    // [56 57 58 59 60 61 62 63 63]
    // [65 66 67 68 69 70 71 72 72]
    // [74 75 76 77 78 79 80 81 81]
    // [74 75 76 77 78 79 80 81 81]

    // auto focal_max = lue::focal_max(array, kernel);

    // static_assert(std::is_same_v<lue::ElementT<decltype(convolve)>, double>);

    // BOOST_REQUIRE_EQUAL(lue::nr_partitions(convolve), 9);
    // BOOST_REQUIRE_EQUAL(convolve.shape(), array_shape);

    // // partition 1
    // {
    //     auto const& partition = convolve.partitions()(0, 0);
    //     auto const data = partition.data(lue::CopyMode::share).get();
    //     BOOST_REQUIRE_EQUAL(shape(data), partition_shape);

    //     BOOST_CHECK_EQUAL(data(0, 0), 4.0);
    //     BOOST_CHECK_EQUAL(data(0, 1), 6.0);
    //     BOOST_CHECK_EQUAL(data(0, 2), 6.0);
    //     BOOST_CHECK_EQUAL(data(1, 0), 6.0);
    //     BOOST_CHECK_EQUAL(data(1, 1), 9.0);
    //     BOOST_CHECK_EQUAL(data(1, 2), 9.0);
    //     BOOST_CHECK_EQUAL(data(2, 0), 6.0);
    //     BOOST_CHECK_EQUAL(data(2, 1), 9.0);
    //     BOOST_CHECK_EQUAL(data(2, 2), 9.0);
    // }
}
