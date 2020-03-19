#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename InputElement,
    Rank rank>
ArrayPartition<std::uint32_t, rank> locality_id_partition(
    ArrayPartition<InputElement, rank> const& input_partition)
{
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    using InputPartition = ArrayPartition<InputElement, rank>;

    using OutputPartition = ArrayPartition<std::uint32_t, rank>;

    return hpx::dataflow(
        hpx::launch::async,

        [](
            InputPartition const& input_partition)
        {
            auto const input_partition_server_ptr{
                hpx::get_ptr(hpx::launch::sync, input_partition)};
            auto const& input_partition_server{*input_partition_server_ptr};

            std::uint32_t const locality_id{hpx::get_locality_id()};

            return OutputPartition{
                hpx::find_here(), input_partition_server.offset(),
                input_partition_server.shape(), locality_id};
        },

        input_partition);
}

}  // namespace detail


template<
    typename InputElement,
    Rank rank>
struct LocalityIDPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::locality_id_partition<InputElement, rank>),
        &detail::locality_id_partition<InputElement, rank>,
        LocalityIDPartitionAction<InputElement, rank>>
{};


template<
    typename InputElement,
    Rank rank>
PartitionedArray<std::uint32_t, rank> locality_id(
    PartitionedArray<InputElement, rank> const& input_array)
{
    // Iterate over all partitions and spawn a task per partition. Each
    // of these tasks runs on the locality the partition is located on and
    // creates a new partition with the same shape as the input partition.
    // Each cell in the new partition is assigned the locality-id of
    // the locality the partition is located on.

    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<std::uint32_t, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LocalityIDPartitionAction<InputElement, rank> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        InputPartition const& input_partition{input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, input_partition](
                        hpx::id_type const locality_id)
                    {
                        return action(locality_id, input_partition);
                    }

                ),
            hpx::get_colocation_id(input_partition.get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
