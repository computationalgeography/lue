#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace detail {

        template<typename Offset, typename Shape>
        Offset offset_in_partitions(Offset const& partition_offset, Shape const& partition_shape)
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
        OutputPartition partition_aggregate_operation_partition(
            Policies const& policies, InputPartition const& input_partition, Functor functor)
        {
            using Offset = OffsetT<InputPartition>;
            using InputData = DataT<InputPartition>;
            using OutputElement = ElementT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [input_partition, policies, functor](
                        Offset const& offset, InputData const& input_partition_data) mutable
                    {
                        AnnotateFunction annotation{"partition_aggregate_operation_partition"};

                        HPX_UNUSED(input_partition);

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
                    }

                    ),
                input_partition.offset(),
                input_partition.data());
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
    PartitionedArray<OutputElementT<Functor>, rank> partition_operation(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        Functor const& functor)
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

        for (Index p = 0; p < nr_partitions; ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies, functor](
                    InputPartition const& input_partition)
                {
                    AnnotateFunction annotation{"partition_aggregate_operation"};

                    return action(locality_id, policies, input_partition, functor);
                },

                input_partitions[p]);
        }

        return OutputArray{
            shape_in_partitions(input_array), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue
