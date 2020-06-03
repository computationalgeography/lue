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

    lue_assert(input_partition.locality_id().get() == hpx::find_here());

    return hpx::dataflow(
        hpx::launch::async,

        [functor](
            InputPartition const& input_partition)
        {
            auto const input_partition_server_ptr{hpx::get_ptr(input_partition).get()};
            auto const& input_partition_server{*input_partition_server_ptr};

            Offset const offset{input_partition_server.offset()};
            InputData const input_partition_data{input_partition_server.data()};
            OutputData output_partition_data{input_partition_data.shape()};

            std::transform(
                input_partition_data.begin(), input_partition_data.end(),
                output_partition_data.begin(), functor);

            return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
        },

        input_partition);
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
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElementT<Functor>, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    lue_assert(all_are_valid(input_array.partitions()));

    detail::UnaryLocalOperationPartitionAction<InputPartition, OutputPartition, Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p)
    {
        InputPartition const& input_partition{input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, functor, input_partition](
                        hpx::id_type const locality_id)
                    {
                        return action(locality_id, input_partition, functor);
                    }

                ),
            input_partition.locality_id());
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
