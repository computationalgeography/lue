#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace detail {

        template<typename Offset, typename Shape>
        auto offset_in_partitions(Offset const& partition_offset, Shape const& partition_shape) -> Offset
        {
            Offset result{partition_offset};
            constexpr Rank rank{lue::rank<Offset>};

            for (std::size_t i = 0; i < rank; ++i)
            {
                lue_hpx_assert(result[i] % partition_shape[i] == 0);
                result[i] /= partition_shape[i];
            }

            return result;
        }


        template<typename Policies, typename InputPartition, typename OutputPartition, typename Functor>
        auto partition_aggregate_operation_partition(
            Policies const& policies, InputPartition const& input_partition, Functor functor)
            -> OutputPartition
        {
            using Offset = OffsetT<InputPartition>;
            using InputData = DataT<InputPartition>;
            using OutputElement = ElementT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,

                [policies, functor](InputPartition const& input_partition) mutable -> OutputPartition
                {
                    AnnotateFunction annotation{"partition_aggregate_operation_partition"};

                    Offset const offset = input_partition.offset(hpx::launch::sync);
                    InputData const input_partition_data = input_partition.data(hpx::launch::sync);

                    auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    Count const nr_elements{lue::nr_elements(input_partition_data)};

                    for (Index idx = 0; idx < nr_elements; ++idx)
                    {
                        if (!indp.is_no_data(input_partition_data, idx))
                        {
                            functor.add_value(input_partition_data[idx]);
                        }
                    }

                    OutputElement result{};

                    if (functor.result_is_valid())
                    {
                        result = functor.result();
                    }
                    else
                    {
                        ondp.mark_no_data(result);
                    }

                    OutputPartition output_partition{
                        hpx::find_here(),
                        offset_in_partitions(offset, input_partition_data.shape()),
                        {1, 1},
                        result};

                    return output_partition;
                },

                input_partition);
        }


        template<typename Policies, typename InputPartition, typename OutputPartition, typename Functor>
        struct PartitionAggregateOperationPartitionAction:
            hpx::actions::make_action<
                decltype(&partition_aggregate_operation_partition<
                         Policies,
                         InputPartition,
                         OutputPartition,
                         Functor>),
                &partition_aggregate_operation_partition<Policies, InputPartition, OutputPartition, Functor>,
                PartitionAggregateOperationPartitionAction<
                    Policies,
                    InputPartition,
                    OutputPartition,
                    Functor>>::type
        {
        };

    }  // namespace detail


    template<typename Policies, typename InputElement, Rank rank, typename Functor>
    auto partition_operation(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        Functor const& functor) -> PartitionedArray<OutputElementT<Functor>, rank>
    {
        // Add a continuation to each partition which asynchronously calls
        // a remote action, passing in the partition client instance and
        // the functor. The result is a partitioned array with a single
        // value in each partition (count, sum, min, max, ...).

        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputElement = OutputElementT<Functor>;
        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        detail::PartitionAggregateOperationPartitionAction<Policies, InputPartition, OutputPartition, Functor>
            action{};

        Localities<rank> localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        Count const nr_partitions{lue::nr_partitions(input_array)};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::async(
                action, localities[partition_idx], policies, input_partitions[partition_idx], functor);
        }

        return OutputArray{
            shape_in_partitions(input_array), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue
