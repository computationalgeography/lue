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
    using Offset = OffsetT<InputPartition>;
    using Shape = ShapeT<InputPartition>;

    using OutputPartition = ArrayPartition<OutputElement, rank>;
    using OutputPartitionData = DataT<OutputPartition>;

    lue_assert(input_partition.is_ready());

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [fill_value](
                    Offset const& offset,
                    Shape const& shape)
                {
                    // Copy the data and move it into a new partition
                    return OutputPartition{hpx::find_here(), offset, OutputPartitionData{shape, fill_value}};
                }

            ),
        input_partition.offset(),
        input_partition.shape());
}

}  // namespace detail


template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
struct ArrayLikePartitionAction:
    hpx::actions::make_action<
        decltype(&detail::array_like_partition<InputElement, OutputElement, rank>),
        &detail::array_like_partition<InputElement, OutputElement, rank>,
        ArrayLikePartitionAction<InputElement, OutputElement, rank>>
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
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    ArrayLikePartitionAction<InputElement, OutputElement, rank> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action](
                InputPartition const& input_partition,
                hpx::shared_future<OutputElement> const& fill_value)
            {
                return action(locality_id, input_partition, fill_value.get());
            },

            input_partitions[p],
            fill_value);
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}


template<
    typename OutputElement,
    typename InputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> array_like(
    PartitionedArray<InputElement, rank> const& input_array,
    OutputElement const fill_value)
{
    return array_like(input_array, hpx::make_ready_future<OutputElement>(fill_value).share());
}

}  // namespace lue
