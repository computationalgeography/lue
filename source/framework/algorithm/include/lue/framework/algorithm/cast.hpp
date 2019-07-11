#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename InputPartition,
    typename OutputElement>
PartitionT<InputPartition, OutputElement> cast_partition(
    InputPartition const& input_partition)
{
    using InputData = DataT<InputPartition>;
    using InputElement = ElementT<InputData>;

    using OutputPartition = PartitionT<InputPartition, OutputElement>;
    using OutputData = DataT<OutputPartition>;

    hpx::future<OutputData> cast = hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(
        [](
            InputData&& input_partition_data)
        {
            OutputData output_partition_data{input_partition_data.shape()};

            std::transform(
                input_partition_data.begin(),
                input_partition_data.end(),
                output_partition_data.begin(),
                [](
                    InputElement const element)
                {
                    return static_cast<OutputElement>(element);
                });

            return output_partition_data;
        }),
        input_partition.data()
    );

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(
        [](
            hpx::naming::id_type const locality,
            OutputData&& output_partition_data)
        {
            return OutputPartition{locality, output_partition_data};
        }),
        hpx::get_colocation_id(input_partition.get_id()),
        cast
    );
}

}  // namespace detail


template<
    typename InputPartition,
    typename OutputElement>
struct CastPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::cast_partition<InputPartition, OutputElement>),
        &detail::cast_partition<InputPartition, OutputElement>,
        CastPartitionAction<InputPartition, OutputElement>>
{};


template<
    typename Array,
    typename OutputElement>
PartitionedArrayT<Array, OutputElement> cast(
    Array const& array)
{
    using InputPartition = PartitionT<Array>;

    using OutputArray = PartitionedArrayT<Array, OutputElement>;
    using OutputPartition = PartitionT<OutputArray, OutputElement>;
    using OutputPartitions = PartitionsT<OutputArray, OutputElement>;

    CastPartitionAction<InputPartition, OutputElement> action;
    OutputPartitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        InputPartition const& input_partition = array.partitions()[p];
        OutputPartition& output_partition = output_partitions[p];

        output_partition = hpx::dataflow(
            hpx::launch::async,
            action,
            // TODO Blocks
            hpx::get_colocation_id(
                hpx::launch::sync, input_partition.get_id()),
            input_partition
        );
    }

    return OutputArray{shape(array), std::move(output_partitions)};
}

}  // namespace lue
