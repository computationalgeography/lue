#define BOOST_TEST_MODULE lue framework core component array_partition
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

using Value = std::int32_t;
static std::size_t const rank = 2;
using Data = lue::ArrayPartitionData<Value, rank>;
using PartitionClient = lue::client::ArrayPartition<Value, Data>;
using PartitionServer = typename PartitionClient::Server;
using Shape = typename PartitionClient::Shape;

}  // Anonymous namespace

LUE_REGISTER_ARRAY_PARTITION(int32_t, 0)
LUE_REGISTER_ARRAY_PARTITION(int32_t, 2)


// BOOST_AUTO_TEST_CASE(default_construct)
// {
//     PartitionClient partition_client =
//         hpx::new_<PartitionClient>(hpx::find_here());
//     std::shared_ptr<PartitionServer> partition_server =
//         partition_client.component();
// 
//     BOOST_REQUIRE(partition_server);
// 
//     Data data{};
//     BOOST_CHECK_EQUAL(partition_server->data(), data);
// }


BOOST_AUTO_TEST_CASE(construct_uninitialized)
{
    Shape shape{{5, 6}};

    PartitionClient partition_client =
        hpx::new_<PartitionClient>(hpx::find_here(), shape);
    std::shared_ptr<PartitionServer> partition_server =
        partition_client.component();

    BOOST_REQUIRE(partition_server);

    // Since the data values are uninitialized, we cannot assume anything
    // about them. Therefore, only look at the definition.
    BOOST_CHECK_EQUAL(partition_server->data().shape(), shape);
}


BOOST_AUTO_TEST_CASE(construct_initialized_with_single_value)
{
    Shape shape{{5, 6}};
    Value value{9};

    PartitionClient partition_client =
        hpx::new_<PartitionClient>(hpx::find_here(), shape, value);
    std::shared_ptr<PartitionServer> partition_server =
        partition_client.component();

    BOOST_REQUIRE(partition_server);

    Data data{shape, value};
    BOOST_CHECK_EQUAL(partition_server->data(), data);
}


BOOST_AUTO_TEST_CASE(scalar_array)
{
    std::size_t const rank = 0;
    using Data = lue::ArrayPartitionData<Value, rank>;
    using PartitionClient = lue::client::ArrayPartition<Value, Data>;
    using PartitionServer = typename PartitionClient::Server;
    using Shape = Data::Shape;

    Shape shape{};
    Value value{9};

    PartitionClient partition_client =
        hpx::new_<PartitionClient>(hpx::find_here(), shape, value);
    std::shared_ptr<PartitionServer> partition_server =
        partition_client.component();

    BOOST_REQUIRE(partition_server);

    Data data{shape, value};
    BOOST_CHECK_EQUAL(partition_server->data(), data);
}
