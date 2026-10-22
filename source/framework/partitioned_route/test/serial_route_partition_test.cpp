#define BOOST_TEST_MODULE lue framework partitioned_route serial_route_partition
#include "stream.hpp"
#include "lue/framework/partitioned_route/serial_route_partition_decl.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(default_construct_client)
{
    // Server is not constructed!

    using RouteID = lue::LargestIntegralElement;

    // Default initialization
    {
        lue::SerialRoutePartition<RouteID, 2> partition;

        BOOST_CHECK(!partition.valid());
        BOOST_CHECK(!partition.is_ready());
    }

    // Value initialization
    {
        lue::SerialRoutePartition<RouteID, 2> partition{};

        BOOST_CHECK(!partition.valid());
        BOOST_CHECK(!partition.is_ready());
    }
}


BOOST_AUTO_TEST_CASE(construct_server01)
{
    using RouteID = lue::SignedIntegralElement<0>;
    using RoutePartition = lue::SerialRoutePartition<RouteID, 2>;

    typename RoutePartition::Offset const partition_offset{33, 44};
    typename RoutePartition::Shape const partition_shape{6, 4};
    typename RoutePartition::RouteFragments fragments{};

    RoutePartition partition_client{
        hpx::find_here(), partition_offset, partition_shape, std::move(fragments)};

    BOOST_CHECK_EQUAL(partition_client.offset(hpx::launch::sync), partition_offset);
    BOOST_CHECK_EQUAL(partition_client.shape(hpx::launch::sync), partition_shape);
    BOOST_CHECK_EQUAL(partition_client.nr_routes(hpx::launch::sync), 0);
}


BOOST_AUTO_TEST_CASE(construct_server02)
{
    using RouteID = lue::UnsignedIntegralElement<lue::nr_unsigned_integral_elements - 1>;
    using RoutePartition = lue::SerialRoutePartition<RouteID, 2>;
    using RouteFragment = typename RoutePartition::RouteFragment;
    using RouteFragments = typename RoutePartition::RouteFragments;

    typename RoutePartition::Offset const partition_offset{33, 44};
    typename RoutePartition::Shape const partition_shape{20, 20};

    RouteFragment const fragment1{{1, 3, 5}};
    RouteFragment const fragment2{{7, 9, 11, 13}};
    RouteFragment const fragment3{{13, 15}};

    std::vector<RouteFragment> fragments0{fragment1};
    std::vector<RouteFragment> fragments1{fragment2, fragment3};

    std::set<RouteID> const route_ids{5, 3};

    RouteFragments fragments{{5, fragments0}, {3, fragments1}};

    std::vector<hpx::id_type> other_localities{hpx::find_remote_localities()};

    // Ideally, some other locality than the current one
    hpx::id_type server_locality{!other_localities.empty() ? other_localities.front() : hpx::find_here()};

    RoutePartition partition_client{server_locality, partition_offset, partition_shape, std::move(fragments)};

    // Wait for the server component to be created in the remote locality before querying it
    partition_client.get();

    BOOST_CHECK_EQUAL(partition_client.offset(hpx::launch::sync), partition_offset);
    BOOST_CHECK_EQUAL(partition_client.shape(hpx::launch::sync), partition_shape);
    BOOST_CHECK_EQUAL(partition_client.nr_routes(hpx::launch::sync), 2);
    BOOST_CHECK_EQUAL(partition_client.nr_route_fragments(hpx::launch::sync), 3);

    BOOST_TEST(partition_client.route_ids(hpx::launch::sync) == route_ids, boost::test_tools::per_element());

    // TODO
    // BOOST_TEST(
    //     partition_client.route_fragments(route_ids[0]).get() == fragments0,
    //     boost::test_tools::per_element());
    // BOOST_TEST(
    //     partition_client.route_fragments(route_ids[1]).get() == fragments1,
    //     boost::test_tools::per_element());
}
