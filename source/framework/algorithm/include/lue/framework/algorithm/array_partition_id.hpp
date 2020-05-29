#pragma once
#include "lue/framework/algorithm/array_like.hpp"


namespace lue {

template<
    typename InputElement,
    Rank rank>
PartitionedArray<std::uint64_t, rank> array_partition_id(
    PartitionedArray<InputElement, rank> const& input_array)
{
    // Iterate over all partitions and spawn a task per partition. Each
    // of these tasks runs on the component the partition is located on and
    // creates a new partition with the same shape as the input partition.
    // Each cell in the new partition is assigned the ID of the partition.
    // This ID equals the linear index of the partition in the array
    // of partitions.

    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputElement = std::uint64_t;
    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    ArrayLikePartitionAction<InputElement, OutputElement, rank> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        InputPartition const& input_partition{input_array.partitions()[p]};

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [action, input_partition, p](
                        hpx::id_type const locality_id)
                    {
                        return action(locality_id, input_partition, p);
                    }

                ),
            input_partition.locality_id());

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
