#define BOOST_TEST_MODULE lue framework core component array_partition
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

using Value = std::int32_t;
static std::size_t const rank = 2;
using PartitionClient = lue::ArrayPartition<Value, rank>;
using Data = typename PartitionClient::Data;
using Shape = typename Data::Shape;

}  // Anonymous namespace


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

    PartitionClient partition =
        hpx::new_<PartitionClient>(hpx::find_here(), shape);
    Data data_we_got = partition.data().get();

    // Since the data values are uninitialized, we cannot assume anything
    // about them. Therefore, only look at the definition.
    BOOST_CHECK_EQUAL(data_we_got.shape(), shape);
}


BOOST_AUTO_TEST_CASE(construct_initialized_with_single_value)
{
    Shape shape{{5, 6}};
    Value value{9};

    PartitionClient partition =
        hpx::new_<PartitionClient>(hpx::find_here(), shape, value);
    Data data_we_got = partition.data().get();

    Data data_we_want{shape, value};
    BOOST_CHECK_EQUAL(data_we_got, data_we_want);
}


BOOST_AUTO_TEST_CASE(scalar_array)
{
    std::size_t const rank = 0;
    using PartitionClient = lue::ArrayPartition<Value, rank>;
    using Data = typename PartitionClient::Data;
    using Shape = Data::Shape;

    Shape shape{};
    Value value{9};

    PartitionClient partition =
        hpx::new_<PartitionClient>(hpx::find_here(), shape, value);
    Data data_we_got = partition.data().get();

    Data data_we_want{shape, value};
    BOOST_CHECK_EQUAL(data_we_got, data_we_want);
}


BOOST_AUTO_TEST_CASE(assignment_operator)
{
    // Construct an empty client instance and assign a new instance to it

    Shape shape{{5, 6}};
    Value value{9};

    {
        PartitionClient partition =
            hpx::new_<PartitionClient>(hpx::find_here(), shape);
        PartitionClient other =
            hpx::new_<PartitionClient>(hpx::find_here(), shape, value);
        partition = other;

        Data data_we_got = partition.data().get();
        Data data_we_want{shape, value};
        BOOST_CHECK_EQUAL(data_we_got, data_we_want);
    }

    {
        // Default initialization
        PartitionClient partition;
        PartitionClient other =
            hpx::new_<PartitionClient>(hpx::find_here(), shape, value);
        partition = other;

        Data data_we_got = partition.data().get();
        Data data_we_want{shape, value};
        BOOST_CHECK_EQUAL(data_we_got, data_we_want);
    }

    {
        // Value initialization
        PartitionClient partition{};
        PartitionClient other =
            hpx::new_<PartitionClient>(hpx::find_here(), shape, value);
        partition = other;

        Data data_we_got = partition.data().get();
        Data data_we_want{shape, value};
        BOOST_CHECK_EQUAL(data_we_got, data_we_want);
    }
}
