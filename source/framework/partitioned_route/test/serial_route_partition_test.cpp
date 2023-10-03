#define BOOST_TEST_MODULE lue framework partitioned_route serial_route_partition
#include "lue/framework/partitioned_route/serial_route_partition.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(default_construct_client)
{
    // Server is not constructed!

    // Default initialization
    {
        lue::SerialRoutePartition<2> partition;

        BOOST_CHECK(!partition.valid());
        BOOST_CHECK(!partition.is_ready());
    }

    // Value initialization
    {
        lue::SerialRoutePartition<2> partition{};

        BOOST_CHECK(!partition.valid());
        BOOST_CHECK(!partition.is_ready());
    }
}


// BOOST_AUTO_TEST_CASE(default_construct_server)
// {
//     lue::SerialRoutePartition<2> partition{hpx::find_here()};
//
//     BOOST_CHECK_EQUAL(partition.nr_routes().get(), 0);
// }


BOOST_AUTO_TEST_CASE(construct_server01)
{
    lue::server::SerialRoutePartition<2>::RouteFragments fragments{};

    hpx::future<hpx::id_type> partition_server_id{
        hpx::new_<lue::server::SerialRoutePartition<2>>(hpx::find_here(), std::move(fragments))};

    lue::SerialRoutePartition<2> partition_client{std::move(partition_server_id)};

    BOOST_CHECK_EQUAL(partition_client.nr_routes().get(), 0);
}


BOOST_AUTO_TEST_CASE(construct_server02)
{
    using RoutePartition = lue::SerialRoutePartition<2>;

    // TODO refactor typing
    lue::SerialRouteFragment<2> const fragment1{{{0, 1}, {2, 3}, {4, 5}}};
    lue::SerialRouteFragment<2> const fragment2{{{6, 7}, {8, 9}, {10, 11}, {12, 13}}};
    lue::SerialRouteFragment<2> const fragment3{{{12, 13}, {14, 15}}};
    std::vector<typename RoutePartition::RouteID> const route_ids{5, 3};

    lue::server::SerialRoutePartition<2>::RouteFragments fragments{
        {route_ids[0], {fragment1}}, {route_ids[1], {fragment2, fragment3}}};

    std::vector<hpx::id_type> other_localities{hpx::find_remote_localities()};

    // Ideally, some other locality than the current one
    hpx::id_type server_locality{!other_localities.empty() ? other_localities.front() : hpx::find_here()};

    lue::SerialRoutePartition<2> partition_client{
        hpx::new_<lue::SerialRoutePartition<2>>(server_locality, std::move(fragments))};

    // Wait for the server component to be created in the remote locality before querying it
    partition_client.wait();

    BOOST_CHECK_EQUAL(partition_client.nr_routes().get(), 2);
    BOOST_CHECK_EQUAL(partition_client.nr_route_fragments().get(), 3);


    BOOST_TEST(partition_client.route_ids().get() == route_ids, boost::test_tools::per_element());

    // TODO hier verder
    // BOOST_TEST(partition_client.route_fragments(3).get() == {fragment1}, boost::test_tools::per_element());
    // BOOST_TEST(partition_client.route_fragments(5).get() == {fragment2, fragment3},
    // boost::test_tools::per_element());
}
