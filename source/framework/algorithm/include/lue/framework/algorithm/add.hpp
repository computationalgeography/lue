#pragma once
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/hpx.hpp>


namespace lue {

client::ArrayPartition<std::int32_t, ArrayPartitionData<std::int32_t, 1>>
    add_partition2(
        client::ArrayPartition<
            std::int32_t,
            ArrayPartitionData<std::int32_t, 1>> const& partition1,
        client::ArrayPartition<
            std::int32_t,
            ArrayPartitionData<std::int32_t, 1>> const& partition2)
{
    using Value = std::int32_t;
    std::size_t const rank = 1;
    using Data = ArrayPartitionData<Value, rank>;
    using ArrayPartition = client::ArrayPartition<Value, Data>;

    // TODO Assert the current locality equals the one of the server
    // auto hpx::naming::get_locality_id_from_id(r.first);

    // Retrieve the partition data
    hpx::shared_future<Data> partition_data1 = partition1.data();
    hpx::shared_future<Data> partition_data2 = partition2.data();

    // Once the data has arrived, sum the values
    hpx::future<Data> addition = hpx::dataflow(
        hpx::util::unwrapping([](
            Data const& partition_data1,
            Data const& partition_data2)
        {
            assert(partition_data1.shape() == partition_data2.shape());

            Data result{partition_data1.shape()};

            std::transform(
                partition_data1.data(),
                partition_data1.data() + partition_data1.size(),
                partition_data2.data(),
                result.data(), std::plus<Value>{});

            return result;
        }),
        partition_data1,
        partition_data2
    );

    // Once the addition has been calculated, create a component containing
    // the result, on the same locality as the first partition passed in
    return addition.then(
        hpx::util::unwrapping([partition1](
            Data&& addition_data)
        {
            return ArrayPartition(partition1.get_id(), addition_data);
        })
    );
}

}  // namespace lue


HPX_PLAIN_ACTION(lue::add_partition2, AddPartitionAction);


namespace lue {

template<
    typename Value,
    std::size_t rank,
    template<typename, std::size_t> typename Data,
    template<typename, typename> typename PartitionedArray
>
PartitionedArray<Value, Data<Value, rank>> add(
    PartitionedArray<Value, Data<Value, rank>>& array1,
    PartitionedArray<Value, Data<Value, rank>>& array2)
{
    assert(array1.shape() == array2.shape());
    assert(array1.partitions().shape() == array2.partitions().shape());
    auto const shape = array1.shape();
    auto const shape_in_partitions = array1.partitions().shape();

    using Additions = PartitionedArray<Value, Data<Value, rank>>;

    typename Additions::Partitions additions_partitions{shape_in_partitions};

    AddPartitionAction add_partition;
    auto const array_partitions1 = array1.partitions();
    auto const array_partitions2 = array2.partitions();

    for(std::size_t p = 0; p < additions_partitions.size(); ++p) {
        additions_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            add_partition,
            hpx::find_here(),  // TODO
            array_partitions1[p],
            array_partitions2[p]);
    }

    return Additions{shape, std::move(additions_partitions)};
}

}  // namespace lue
