#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename Partition>
Partition add_partition(
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

    using Data = DataT<Partition>;
    using Element = ElementT<Partition>;

    // Asynchronously retrieve the partition data from the array partition
    // components
    hpx::shared_future<Data> partition_data1 = partition1.data();
    hpx::shared_future<Data> partition_data2 = partition2.data();

    // Once the data has arrived, sum the values
    hpx::future<Data> addition = hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping([](
            Data const& partition_data1,
            Data const& partition_data2)
        {
            assert(partition_data1.shape() == partition_data2.shape());

            Data result{partition_data1.shape()};

            std::transform(
                partition_data1.begin(),
                partition_data1.end(),
                partition_data2.begin(),
                result.begin(), std::plus<Element>{});

            return result;
        }),
        partition_data1,
        partition_data2
    );

    // Once the addition has been calculated, create a new component
    // containing the result, on the same locality as the first partition
    // passed in
    return addition.then(
        // TODO Pass in ref to partition?
        hpx::util::unwrapping([partition1](
            Data&& addition_data)
        {
            return Partition(partition1.get_id(), addition_data);
        })
    );
}

}  // namespace detail


template<
    typename Partition>
struct AddPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::add_partition<Partition>),
        &detail::add_partition<Partition>,
        AddPartitionAction<Partition>>
{};


template<
    typename Array>
Array add(
    Array const& array1,
    Array const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using Partitions = PartitionsT<Array>;
    using Partition = PartitionT<Array>;

    AddPartitionAction<Partition> add_partition_action;

    Partitions partitions{shape_in_partitions(array1)};

    // Attach a continuation to each pair of input partitions that adds
    // all elements in those partitions and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        Partition const& input_partition1 = array1.partitions()[p];
        Partition const& input_partition2 = array2.partitions()[p];
        Partition& output_partition = partitions[p];

        output_partition = hpx::dataflow(
            hpx::launch::async,
            add_partition_action,
            hpx::get_colocation_id(
                hpx::launch::sync, input_partition1.get_id()),
            input_partition1,
            input_partition2);
    }

    return Array{shape(array1), std::move(partitions)};
}

}  // namespace lue
