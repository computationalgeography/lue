#define BOOST_TEST_MODULE lue framework partitioned_array array_partition
#include "lue/framework/partitioned_array/array_partition_decl.hpp"
#include "lue/framework/partitioned_array/array_partition_data.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using Value = std::int32_t;
    static std::size_t const rank = 2;
    using PartitionClient = lue::ArrayPartition<Value, rank>;
    using Data = typename PartitionClient::Data;
    using Offset = PartitionClient::Offset;
    using Shape = typename Data::Shape;

}  // Anonymous namespace


// BOOST_AUTO_TEST_CASE(default_construct)
// {
//     PartitionClient partition_client{hpx::find_here()};
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
    Offset offset{3, 4};
    Shape shape{{5, 6}};

    PartitionClient partition{hpx::find_here(), offset, shape};
    Data data_we_got = partition.data().get();

    // Since the data values are uninitialized, we cannot assume anything
    // about them. Therefore, only look at the definition.
    BOOST_CHECK_EQUAL(data_we_got.shape(), shape);
}


BOOST_AUTO_TEST_CASE(construct_initialized_with_single_value)
{
    Offset offset{3, 4};
    Shape shape{{5, 6}};
    Value value{9};

    PartitionClient partition{hpx::find_here(), offset, shape, value};
    Data data_we_got = partition.data().get();

    Data data_we_want{shape, value};
    BOOST_CHECK_EQUAL(data_we_got, data_we_want);
}


// BOOST_AUTO_TEST_CASE(scalar_array)
// {
//     std::size_t const rank = 0;
//     using PartitionClient = lue::ArrayPartition<Value, rank>;
//     using Data = typename PartitionClient::Data;
//     using Offset = typename PartitionClient::Offset;
//     using Shape = Data::Shape;
//
//     Offset offset{};
//     Shape shape{};
//     Value value{9};
//
//     PartitionClient partition{hpx::find_here(), offset, shape, value};
//     Data data_we_got = partition.data().get();
//
//     Data data_we_want{shape, value};
//     BOOST_CHECK_EQUAL(data_we_got, data_we_want);
// }


BOOST_AUTO_TEST_CASE(assignment_operator)
{
    // Construct an empty client instance and assign a new instance to it

    Offset offset{3, 4};
    Shape shape{{5, 6}};
    Value value{9};

    {
        PartitionClient partition{hpx::find_here(), offset, shape};
        PartitionClient other{hpx::find_here(), offset, shape, value};
        partition = other;

        Data data_we_got = partition.data().get();
        Data data_we_want{shape, value};
        BOOST_CHECK_EQUAL(data_we_got, data_we_want);
    }

    {
        // Default initialization
        PartitionClient partition;
        PartitionClient other{hpx::find_here(), offset, shape, value};
        partition = other;

        Data data_we_got = partition.data().get();
        Data data_we_want{shape, value};
        BOOST_CHECK_EQUAL(data_we_got, data_we_want);
    }

    {
        // Value initialization
        PartitionClient partition{};
        PartitionClient other{hpx::find_here(), offset, shape, value};
        partition = other;

        Data data_we_got = partition.data().get();
        Data data_we_want{shape, value};
        BOOST_CHECK_EQUAL(data_we_got, data_we_want);
    }
}


BOOST_AUTO_TEST_CASE(emulate_promise_future)
{
    // 1. Create a component client instance without a connection with
    //    a component server instance ('dangling').
    //    This instance can be returned to the caller, which can add
    //    continuations to it.
    // 2. Create a component client instance with a connection with a
    //    component server instance.
    //    Do whatever it takes to calculate the values to store in the
    //    component server instance.
    // 3. Connect the component client instance created in step 1 with
    //    the component server instance created in step 2.

    // Reasons for not just returning a client instance that will become
    // ready after some continuation has finished executing is that such
    // a continuation cannot be written easily. In some algorithms that
    // result in array of client instances:
    // - The time it takes to calculate instances differ a lot. The next
    //   algorithm adds continuations to the client instances created by the
    //   current algorithm, and we want tasks that are ready to run to be
    //   scheduled as soon as possible.
    // - Component server instances depend on neighbouring instances
    //   (8 in 2D case). Which instances exactly is determined at runtime
    //   and changes while iterating to a solution.
    //
    // There needs to be some kind of solve function that manages the
    // calculations and marks resulting client instances as ready.

    {
        // Create a promise to a component ID
        hpx::promise<hpx::id_type> component_id_p;

        // Create a client instance for which the server instance will be
        // created later. Pass in a future to the ID of the component that
        // will be created.
        PartitionClient partition1{component_id_p.get_future()};
        BOOST_CHECK(partition1.valid());
        BOOST_CHECK(!partition1.is_ready());

        auto solve = [component_id_p = std::move(component_id_p)]() mutable
        {
            // Create a server instance
            Offset offset{3, 4};
            Shape shape{{5, 6}};
            PartitionClient partition2{hpx::find_here(), offset, shape};
            BOOST_CHECK(partition2.valid());

            // Pass the ID of the server instance to partition1
            component_id_p.set_value(partition2.get_id());
        };

        solve();

        BOOST_CHECK(partition1.valid());
        partition1.get();
        BOOST_CHECK(partition1.is_ready());
    }

    {
        // Verify that adding a continuation before setting the value
        // on the promise works
        hpx::promise<hpx::id_type> component_id_p;
        PartitionClient partition1{component_id_p.get_future()};
        BOOST_CHECK(partition1.valid());
        BOOST_CHECK(!partition1.is_ready());

        hpx::future<lue::Count> answer = partition1.then([]([[maybe_unused]] PartitionClient&& client)
                                                         { return lue::nr_elements(client.shape().get()); });

        BOOST_CHECK(answer.valid());
        BOOST_CHECK(!answer.is_ready());

        auto solve = [component_id_p = std::move(component_id_p)]() mutable
        {
            Offset offset{3, 4};
            Shape shape{{5, 6}};
            PartitionClient partition2{hpx::find_here(), offset, shape};
            BOOST_CHECK(partition2.valid());

            component_id_p.set_value(partition2.get_id());
        };

        solve();

        BOOST_CHECK_EQUAL(answer.get(), 5 * 6);
    }
}
