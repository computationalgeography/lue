#pragma once
#include "lue/framework/algorithm/array_like.hpp"
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
    // We don't need support for detecting / marking no-data here
    using OutputElement = std::uint32_t;
    using Policies = policy::array_like::DefaultPolicies<OutputElement, InputElement>;

    return array_like_partition(Policies{}, input_partition, hpx::get_locality_id());
}

}  // namespace detail


template<
    typename InputElement,
    Rank rank>
struct LocalityIDPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::locality_id_partition<InputElement, rank>),
        &detail::locality_id_partition<InputElement, rank>,
        LocalityIDPartitionAction<InputElement, rank>>::type
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
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<std::uint32_t, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LocalityIDPartitionAction<InputElement, rank> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action](
                InputPartition const& input_partition)
            {
                AnnotateFunction annotation{"locality_id"};

                return action(locality_id, input_partition);
            },

            input_partitions[p]);
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}

}  // namespace lue
