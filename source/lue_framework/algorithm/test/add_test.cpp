#define BOOST_TEST_MODULE lue framework algorithm add
#include "lue/framework/algorithm/add.hpp"
#include "lue/framework/algorithm/unique_id.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

// using Value = std::int32_t;
// static std::size_t const rank = 2;
// using Data = lue::ArrayPartitionData<Value, rank>;
// using PartitionClient = lue::client::ArrayPartition<Value, Data>;
// using PartitionServer = typename PartitionClient::Server;
// using Definition = typename PartitionClient::Definition;

}  // Anonymous namespace

LUE_REGISTER_ARRAY_PARTITION(int32_t, 2)


BOOST_AUTO_TEST_CASE(array_array_2d)
{
    using Value = std::int32_t;
    std::size_t const rank = 2;
    using Data = lue::ArrayPartitionData<Value, rank>;
    // using Definition = typename Data::Definition;
    // using Shape = typename Definition::Shape;

    // typename Data::index const nr_rows = 600;
    // typename Data::index const nr_cols = 400;

    // Definition definition{Shape{{nr_rows, nr_cols}}};

    // // Create input arrays
    // lue::PartitionedArray<std::int32_t, Data> a1{definition};
    // lue::PartitionedArray<std::int32_t, Data> a2{definition};

    // lue::unique_id(a1);
    // lue::unique_id(a2);

    // Calculate output array

    // auto result = lue::framework::add(a1, a2);
    // result.wait();

    // Verify output array

    // All 9


}


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


// BOOST_AUTO_TEST_CASE(construct_uninitialized)
// {
//     Definition definition{
//         typename Definition::Start{{3, 4}},
//         typename Definition::Shape{{5, 6}}
//     };
// 
//     PartitionClient partition_client =
//         hpx::new_<PartitionClient>(hpx::find_here(), definition);
//     std::shared_ptr<PartitionServer> partition_server =
//         partition_client.component();
// 
//     BOOST_REQUIRE(partition_server);
// 
//     // Since the data values are uninitialized, we cannot assume anything
//     // about them. Therefore, only look at the definition.
//     BOOST_CHECK_EQUAL(partition_server->data().definition(), definition);
// }
// 
// 
// BOOST_AUTO_TEST_CASE(construct_initialized_with_single_value)
// {
//     Definition definition{
//         typename Definition::Start{{3, 4}},
//         typename Definition::Shape{{5, 6}}
//     };
//     Value value{9};
// 
//     PartitionClient partition_client =
//         hpx::new_<PartitionClient>(hpx::find_here(), definition, value);
//     std::shared_ptr<PartitionServer> partition_server =
//         partition_client.component();
// 
//     BOOST_REQUIRE(partition_server);
// 
//     Data data{definition, value};
//     BOOST_CHECK_EQUAL(partition_server->data(), data);
// }
