#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/component.hpp"


namespace lue {
namespace detail {

template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
OutputPartition unary_local_operation_partition(
    InputPartition const& input_partition,
    Functor const& functor)
{
    using Offset = OffsetT<InputPartition>;
    using InputData = DataT<InputPartition>;
    using OutputData = DataT<OutputPartition>;

    lue_assert(input_partition.is_ready());

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [input_partition, functor](
                    Offset const& offset,
                    InputData const& input_partition_data)
                {
                    AnnotateFunction annotation{"unary_local_operation_partition"};

                    HPX_UNUSED(input_partition);

                    OutputData output_partition_data{input_partition_data.shape()};

                    std::transform(
                        input_partition_data.begin(), input_partition_data.end(),
                        output_partition_data.begin(), functor);

                    return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                }

            ),
            input_partition.offset(),
            input_partition.data()
        );
}


template<
    typename InputPartition,
    typename OutputPartition,
    typename Functor>
struct UnaryLocalOperationPartitionAction:
    hpx::actions::make_action<
            decltype(&unary_local_operation_partition<InputPartition, OutputPartition, Functor>),
            &unary_local_operation_partition<InputPartition, OutputPartition, Functor>,
            UnaryLocalOperationPartitionAction<InputPartition, OutputPartition, Functor>
        >
{};

}  // namespace detail


template<
    typename InputElement,
    Rank rank,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> unary_local_operation(
    PartitionedArray<InputElement, rank> const& input_array,
    Functor const& functor)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    lue_assert(all_are_valid(input_array.partitions()));

    detail::UnaryLocalOperationPartitionAction<InputPartition, OutputPartition, Functor> action;

    Localities<rank> const& localities{input_array.localities()};
    InputPartitions const& input_partitions{input_array.partitions()};
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [locality_id=localities[p], action, functor](
                InputPartition const& input_partition)
            {
                AnnotateFunction annotation{"unary_local_operation"};

                return action(locality_id, input_partition, functor);
            },

            input_partitions[p]);
    }

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}

}  // namespace lue
