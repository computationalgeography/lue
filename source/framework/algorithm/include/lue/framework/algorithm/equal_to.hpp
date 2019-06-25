#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename Partition>
PartitionT<Partition, bool> equal_to_partition(
    Partition const& partition1,
    Partition const& partition2)
{
    // Obtain the data from both partitions and calculate the values
    // for the new partition

    // Arguments are client instances of components that are ready. Asking
    // for the component values involves copying the data out of the
    // components. In case components lives on the same locality as
    // we do, this copying is not necessary.
    // TODO Optimize for this

    using ResultPartition = PartitionT<Partition, bool>;
    using Data = DataT<Partition>;
    using ResultData = DataT<Partition, bool>;
    using Element = ElementT<Partition>;

    // Asynchronously retrieve the partition data from the array partition
    // components
    hpx::shared_future<Data> partition_data1 = partition1.data();
    hpx::shared_future<Data> partition_data2 = partition2.data();

    // Once the data has arrived, compare the values
    hpx::future<ResultData> result_data = hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping([](
            Data const& partition_data1,
            Data const& partition_data2)
        {
            assert(partition_data1.shape() == partition_data2.shape());

            ResultData result{partition_data1.shape()};

            std::transform(
                partition_data1.begin(),
                partition_data1.end(),
                partition_data2.begin(),
                result.begin(), std::equal_to<Element>{});

            return result;
        }),
        partition_data1,
        partition_data2
    );

    // Once the result has been calculated, create a new component
    // containing the data, on the same locality as the first partition
    // passed in
    return result_data.then(
        // TODO Pass in ref to partition?
        hpx::util::unwrapping([partition1](
            ResultData&& data)
        {
            return ResultPartition(
                partition1.get_id(), data);
        })
    );
}

}  // namespace detail


template<
    typename Partition>
struct EqualToPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::equal_to_partition<Partition>),
        &detail::equal_to_partition<Partition>,
        EqualToPartitionAction<Partition>>
{};


template<
    typename Array>
PartitionedArrayT<Array, bool> equal_to(
    Array const& array1,
    Array const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using InputPartition = PartitionT<Array>;

    using OutputArray = PartitionedArrayT<Array, bool>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    EqualToPartitionAction<InputPartition> equal_to_partition_action;

    OutputPartitions output_partitions{shape_in_partitions(array1)};

    // Attach a continuation to each pair of input partitions that compares
    // all elements in those partitions and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        InputPartition const& input_partition1 = array1.partitions()[p];
        InputPartition const& input_partition2 = array2.partitions()[p];
        OutputPartition& output_partition = output_partitions[p];

        output_partition = hpx::dataflow(
            hpx::launch::async,
            equal_to_partition_action,
            hpx::get_colocation_id(
                hpx::launch::sync, input_partition1.get_id()),
            input_partition1,
            input_partition2);

    }

    return OutputArray{shape(array1), std::move(output_partitions)};
}

}  // namespace lue
