#define BOOST_TEST_MODULE lue framework partitioned_route serial_route
#include "lue/framework/serial_route.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/stream.hpp"


BOOST_AUTO_TEST_CASE(default_construct)
{
    using Route = lue::SerialRoute<std::uint32_t, 2>;
    using Shape = typename Route::Shape;

    // Default initialization
    {
        Route route;

        BOOST_CHECK_EQUAL(route.shape(), Shape{});
        BOOST_CHECK(!route.valid());
    }

    // Value initialization
    {
        Route route{};

        BOOST_CHECK_EQUAL(route.shape(), Shape{});
        BOOST_CHECK(!route.valid());
    }
}


// BOOST_AUTO_TEST_CASE(construct)
// {
//     using Route = lue::SerialRoute<std::int64_t, 2>;
//     using Shape = typename Route::Shape;
//     using Starts = typename Route::Starts;
//     using Partitions = typename Route::Partitions;
//
//     Shape const array_shape{60, 40};
//     Shape const shape_in_partitions{6, 4};
//
//     hpx::future<Starts> starts_f{hpx::make_ready_future<Starts>()};
//
//     // TODO Won't work if partitions aren't valid. SerialRoute checks for that.
//
//     // Partitions partitions{shape_in_partitions};
//
//     // Route route{array_shape, std::move(starts_f), std::move(partitions)};
//
//     // BOOST_CHECK_EQUAL(route.shape(), array_shape);
//     // BOOST_CHECK(route.valid());
//     // BOOST_CHECK_EQUAL(route.partitions().shape(), shape_in_partitions);
//     // route.starts().wait();
//     // BOOST_CHECK(route.is_ready());
//     // BOOST_CHECK_EQUAL(route.nr_routes(), 0);
// }
