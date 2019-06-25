#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return a new copy the array partition passed in
    @tparam     Partition Client class of partition component
    @param      partition Client of array partition component
    @return     A copy of the array partition passed in
*/
template<
    typename Partition>
Partition copy_partition(
    Partition const& partition)
{
    // Assert the locality of the partition is the same as the locality
    // this code runs on
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    // TODO Generalize. It is possible this code runs on a different
    //     partition than the one the partition passed in is located on.
    //     In that case the default copy doesn't work, does it? Implement
    //     the copy constructor.

    return Partition{partition};
}

}  // namespace detail


template<
    typename Partition>
struct CopyPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::copy_partition<Partition>),
        &detail::copy_partition<Partition>,
        CopyPartitionAction<Partition>>
{};


template<
    typename Array>
Array copy(
    Array const& array)
{
    using Partition = PartitionT<Array>;
    using Partitions = PartitionsT<Array>;

    CopyPartitionAction<Partition> copy_partition_action;

    // Create a new array for storing the new partitions. These partitions
    // will contain the copied elements.
    Partitions partitions{shape_in_partitions(array)};

    // Attach a continuation to each partition that copies all input
    // elements to the output elements. These continuations run on the
    // same localities as where the partitions themselves are located.
    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition const& source_partition = array.partitions()[p];
        Partition& destination_partition = partitions[p];

        destination_partition = hpx::dataflow(
            hpx::launch::async,
            copy_partition_action,
            hpx::get_colocation_id(
                hpx::launch::sync, source_partition.get_id()),
            source_partition
        );

    }

    return Array{shape(array), std::move(partitions)};
}

}  // namespace lue
