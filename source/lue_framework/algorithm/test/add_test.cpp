#define BOOST_TEST_MODULE lue framework algorithm add
#include "lue/framework/algorithm/add.hpp"
#include "lue/framework/algorithm/fill.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


BOOST_AUTO_TEST_CASE(array_array_1d)
{
    using Value = std::int32_t;
    std::size_t const rank = 1;
    using Data = lue::ArrayPartitionData<Value, rank>;
    using Array = lue::PartitionedArray<std::int32_t, Data>;
    using Shape = typename Data::Shape;
    using Index = typename Data::Index;

    Index const nr_elements = 100;
    Shape const shape{{nr_elements}};

    lue::PartitionedArray<Value, Data> array1{shape};
    lue::PartitionedArray<Value, Data> array2{shape};

    hpx::shared_future<Value> fill_value1 = hpx::make_ready_future<Value>(5);
    hpx::shared_future<Value> fill_value2 = hpx::make_ready_future<Value>(6);

    // Request the filling of the arrays and wait for it to finish
    hpx::future<void> filled1 = lue::fill(array1, fill_value1);
    hpx::future<void> filled2 = lue::fill(array2, fill_value2);
    hpx::wait_all(filled1, filled2);

    auto result = lue::add(array1, array2);

    for(auto const& partition: result.partitions()) {
        auto partition_data = partition.data().get();

        BOOST_CHECK_EQUAL(
            std::accumulate(
                partition_data.data(),
                partition_data.data() + partition_data.size(), 0),
            nr_elements * (5 + 6));
    }
}
