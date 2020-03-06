#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return a new copy the array partition passed in
    @tparam     Partition Client class of partition component
    @param      input_partition Client of array partition component
    @return     A copy of the array partition passed in
*/
template<
    typename Partition>
Partition copy_partition(
    Partition const& input_partition)
{
    // Assert the locality of the partition is the same as the locality
    // this code runs on
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    // Copy the data and move it into a new partition
    auto const input_partition_server_ptr{hpx::get_ptr(input_partition).get()};
    auto const& input_partition_server{*input_partition_server_ptr};

    auto offset{input_partition_server.offset()};
    auto input_partition_data = input_partition_server.data(CopyMode::copy);

    return Partition{
        hpx::find_here(), offset, std::move(input_partition_data)};
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


/*!
    @brief      Return the result of copying a partitioned array
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input array
    @param      array Partitioned array to copy
    @return     New partitioned array
*/
template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> copy(
    PartitionedArray<Element, rank> const& input_array)
{
    using InputArray = PartitionedArray<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<Element, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    CopyPartitionAction<InputPartition> action;
    OutputPartitions output_partitions{
        shape_in_partitions(input_array), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                InputPartition const& input_partition,
                hpx::future<hpx::id_type>&& locality_id)
            {
                return action(
                    locality_id.get(),
                    input_partition);
            },

            input_array.partitions()[p],
            hpx::get_colocation_id(input_array.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
