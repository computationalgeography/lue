#pragma once
#include "lue/framework/algorithm/array_like.hpp"


namespace lue {

    template<typename IDElement, typename InputElement, Rank rank>
    auto array_partition_id(PartitionedArray<InputElement, rank> const& input_array)
        -> PartitionedArray<IDElement, rank>
    {
        // Iterate over all partitions and spawn a task per partition. Each
        // of these tasks runs on the component the partition is located on and
        // creates a new partition with the same shape as the input partition.
        // Each cell in the new partition is assigned the ID of the partition.
        // This ID equals the linear index of the partition in the array
        // of partitions.

        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputElement = IDElement;
        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        // We don't need support for detecting / marking no-data here
        using Policies = policy::array_like::DefaultPolicies<OutputElement, InputElement>;
        Policies policies{};

        ArrayLikePartitionAction<Policies, InputElement, OutputElement, rank> action;

        Localities<rank> localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies, p](InputPartition const& input_partition)
                {
                    AnnotateFunction annotation{"array_partition_id"};

                    return action(locality_id, policies, input_partition, static_cast<IDElement>(p));
                },

                input_partitions[p]);
        }

        return OutputArray{shape(input_array), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue
