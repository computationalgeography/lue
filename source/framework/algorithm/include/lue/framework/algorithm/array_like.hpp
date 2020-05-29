#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
ArrayPartition<OutputElement, rank> array_like_partition(
    ArrayPartition<InputElement, rank> const& input_partition,
    OutputElement const fill_value)
{
    using InputPartition = ArrayPartition<InputElement, rank>;
    using OutputPartition = ArrayPartition<OutputElement, rank>;
    using OutputPartitionData = DataT<OutputPartition>;

    lue_assert(input_partition.locality_id().get() == hpx::find_here());

    return hpx::dataflow(
        hpx::launch::async,

        [fill_value](
            InputPartition const& input_partition)
        {
            // Copy the data and move it into a new partition
            auto const input_partition_server_ptr{
                hpx::get_ptr(input_partition).get()};
            auto const& input_partition_server{
                *input_partition_server_ptr};

            auto const offset{input_partition_server.offset()};
            auto const shape{input_partition_server.shape()};

            return OutputPartition{
                hpx::find_here(), std::move(offset),
                OutputPartitionData{std::move(shape), fill_value}};
        },

        input_partition);
}

}  // namespace detail


template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
struct ArrayLikePartitionAction:
    hpx::actions::make_action<
        decltype(&detail::array_like_partition<
            InputElement, OutputElement, rank>),
        &detail::array_like_partition<
            InputElement, OutputElement, rank>,
        ArrayLikePartitionAction<
            InputElement, OutputElement, rank>>
{};


template<
    typename OutputElement,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> array_like(
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<OutputElement> const& fill_value)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    ArrayLikePartitionAction<InputElement, OutputElement, rank> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        InputPartition const& input_partition{input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, input_partition](
                        hpx::id_type const locality_id,
                        OutputElement const fill_value)
                    {
                        return action(locality_id, input_partition, fill_value);
                    }

                ),

            input_partition.locality_id(),
            fill_value);
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}


template<
    typename OutputElement,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> array_like(
    PartitionedArray<InputElement, rank> const& input_array,
    OutputElement const fill_value)
{
    return array_like(
        input_array,
        hpx::make_ready_future<OutputElement>(fill_value).share());
}

}  // namespace lue
