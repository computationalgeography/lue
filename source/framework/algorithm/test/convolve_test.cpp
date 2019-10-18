#define BOOST_TEST_MODULE lue framework algorithm convolve
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/convolve.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


BOOST_AUTO_TEST_CASE(window_total_1d)
{
    using Element = std::int32_t;
    std::size_t const rank = 1;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{5}};
    Shape const partition_shape{{2}};

    // TODO Make the partitions a bit larger, to get a section in each
    //     partition where the kernel is used

    // +---+---+---+---+---+
    // | 1 | 1 | 1 | 1 | 1 |
    // +---+---+---+---+---+
    Array array{array_shape, partition_shape};
    BOOST_REQUIRE_EQUAL(lue::nr_partitions(array), 3);
    lue::fill(array, hpx::make_ready_future<Element>(1).share()).wait();

    // +------+------+------+
    // | true | true | true |
    // +------+------+------+
    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    // +---+---+---+---+---+
    // | 2 | 3 | 3 | 3 | 2 |
    // +---+---+---+---+---+
    auto convolve = lue::convolve(array, kernel);

    static_assert(std::is_same_v<lue::ElementT<decltype(convolve)>, double>);

    BOOST_REQUIRE_EQUAL(lue::nr_partitions(convolve), 3);
    BOOST_REQUIRE_EQUAL(convolve.shape(), array_shape);

    // TODO Left border
    // {
    //     auto const& partition = convolve.partitions()[0];
    //     auto const data = partition.data(lue::CopyMode::share).get();
    //     BOOST_REQUIRE_EQUAL(data.size(), 2);
    //     BOOST_CHECK_EQUAL(data[0], 2.0);
    //     BOOST_CHECK_EQUAL(data[1], 3.0);
    // }

    {
        auto const& partition = convolve.partitions()[1];
        auto const data = partition.data(lue::CopyMode::share).get();
        BOOST_REQUIRE_EQUAL(data.size(), 2);
        BOOST_CHECK_EQUAL(data[0], 3.0);
        BOOST_CHECK_EQUAL(data[1], 3.0);
    }

    // TODO Right border
    // {
    //     auto const& partition = convolve.partitions()[1];
    //     auto const data = partition.data(lue::CopyMode::share).get();
    //     BOOST_REQUIRE_EQUAL(data.size(), 1);
    //     BOOST_CHECK_EQUAL(data[0], 2.0);
    // }
}


BOOST_AUTO_TEST_CASE(window_total_2d)
{
    using Element = std::int32_t;
    std::size_t const rank = 2;

    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{2, 2}};

    // +---+---+---+---+---+
    // | 1 | 1 | 1 | 1 | 1 |
    // +---+---+---+---+---+
    // | 1 | 1 | 1 | 1 | 1 |
    // +---+---+---+---+---+
    // | 1 | 1 | 1 | 1 | 1 |
    // +---+---+---+---+---+
    // | 1 | 1 | 1 | 1 | 1 |
    // +---+---+---+---+---+
    // | 1 | 1 | 1 | 1 | 1 |
    // +---+---+---+---+---+
    Array array{array_shape, partition_shape};
    lue::fill(array, hpx::make_ready_future<Element>(1).share()).wait();

    // +------+------+------+
    // | true | true | true |
    // +------+------+------+
    // | true | true | true |
    // +------+------+------+
    // | true | true | true |
    // +------+------+------+
    auto const kernel = lue::box_kernel<bool, rank>(1, true);

    // +---+---+---+---+---+
    // | 4 | 6 | 6 | 6 | 4 |
    // +---+---+---+---+---+
    // | 6 | 9 | 9 | 9 | 6 |
    // +---+---+---+---+---+
    // | 6 | 9 | 9 | 9 | 6 |
    // +---+---+---+---+---+
    // | 6 | 9 | 9 | 9 | 6 |
    // +---+---+---+---+---+
    // | 4 | 6 | 6 | 6 | 4 |
    // +---+---+---+---+---+
    auto convolve = lue::convolve(array, kernel);

    static_assert(std::is_same_v<lue::ElementT<decltype(convolve)>, double>);

    // BOOST_REQUIRE_EQUAL(lue::nr_partitions(convolve), 9);
    // BOOST_REQUIRE_EQUAL(convolve.shape(), array_shape);

    // auto const& partition = convolve.partitions()[0];
    // auto const data = partition.data(lue::CopyMode::share).get();
    // BOOST_REQUIRE_EQUAL(data.size(), 25);

    // BOOST_CHECK_EQUAL(data[0], 4.0);
    // BOOST_CHECK_EQUAL(data[1], 6.0);
    // BOOST_CHECK_EQUAL(data[2], 6.0);
    // BOOST_CHECK_EQUAL(data[3], 6.0);
    // BOOST_CHECK_EQUAL(data[4], 4.0);

    // BOOST_CHECK_EQUAL(data[5], 6.0);
    // BOOST_CHECK_EQUAL(data[6], 9.0);
    // BOOST_CHECK_EQUAL(data[7], 9.0);
    // BOOST_CHECK_EQUAL(data[8], 9.0);
    // BOOST_CHECK_EQUAL(data[9], 6.0);

    // BOOST_CHECK_EQUAL(data[10], 6.0);
    // BOOST_CHECK_EQUAL(data[11], 9.0);
    // BOOST_CHECK_EQUAL(data[12], 9.0);
    // BOOST_CHECK_EQUAL(data[13], 9.0);
    // BOOST_CHECK_EQUAL(data[14], 6.0);

    // BOOST_CHECK_EQUAL(data[15], 6.0);
    // BOOST_CHECK_EQUAL(data[16], 9.0);
    // BOOST_CHECK_EQUAL(data[17], 9.0);
    // BOOST_CHECK_EQUAL(data[18], 9.0);
    // BOOST_CHECK_EQUAL(data[19], 6.0);

    // BOOST_CHECK_EQUAL(data[20], 4.0);
    // BOOST_CHECK_EQUAL(data[21], 6.0);
    // BOOST_CHECK_EQUAL(data[22], 6.0);
    // BOOST_CHECK_EQUAL(data[23], 6.0);
    // BOOST_CHECK_EQUAL(data[24], 4.0);
}
