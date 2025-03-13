#pragma once
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/unary_local_operation.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename Policies, typename InputPartition, typename OutputPartition, typename Functor>
        auto unary_local_operation_partition(
            Policies const& policies,
            InputPartition const& input_partition,
            Functor const& functor) -> OutputPartition
        {
            using Offset = OffsetT<InputPartition>;
            using InputData = DataT<InputPartition>;
            using OutputData = DataT<OutputPartition>;

            lue_hpx_assert(input_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [input_partition, policies, functor](
                        Offset const& offset, InputData const& input_partition_data)
                    {
                        AnnotateFunction const annotation{
                            std::format("{}: partition", functor_name<Functor>)};

                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{input_partition_data.shape()};

                        auto const& dp = policies.domain_policy();
                        auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
                        auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                        Count const nr_elements{lue::nr_elements(input_partition_data)};

                        for (Index element_idx = 0; element_idx < nr_elements; ++element_idx)
                        {
                            if (indp.is_no_data(input_partition_data, element_idx))
                            {
                                ondp.mark_no_data(output_partition_data, element_idx);
                            }
                            else if (!dp.within_domain(input_partition_data[element_idx]))
                            {
                                ondp.mark_no_data(output_partition_data, element_idx);
                            }
                            else
                            {
                                output_partition_data[element_idx] =
                                    functor(input_partition_data[element_idx]);

                                if (!rp.within_range(
                                        input_partition_data[element_idx],
                                        output_partition_data[element_idx]))
                                {
                                    ondp.mark_no_data(output_partition_data, element_idx);
                                }
                            }
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                    ),
                input_partition.offset(),
                input_partition.data());
        }


        template<typename Policies, typename InputPartition, typename OutputPartition, typename Functor>
        struct UnaryLocalOperationPartitionAction:
            hpx::actions::make_action<
                decltype(&unary_local_operation_partition<
                         Policies,
                         InputPartition,
                         OutputPartition,
                         Functor>),
                &unary_local_operation_partition<Policies, InputPartition, OutputPartition, Functor>,
                UnaryLocalOperationPartitionAction<Policies, InputPartition, OutputPartition, Functor>>::type
        {
        };

    }  // namespace detail


    // unary_local_operation(array)
    template<typename Policies, Rank rank, typename Functor>
    auto unary_local_operation(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        using InputElement = policy::InputElementT<Policies, 0>;
        using OutputElement = policy::OutputElementT<Policies, 0>;

        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        AnnotateFunction const annotation{std::format("{}: array", functor_name<Functor>)};

        lue_hpx_assert(all_are_valid(input_array.partitions()));

        detail::UnaryLocalOperationPartitionAction<Policies, InputPartition, OutputPartition, Functor> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index partition_idx = 0; partition_idx < nr_partitions(input_array); ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[partition_idx], action, policies, functor](
                    InputPartition const& input_partition)
                { return action(locality_id, policies, input_partition, functor); },

                input_partitions[partition_idx]);
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }


    // unary_local_operation(scalar)
    template<typename Policies, typename Functor>
    auto unary_local_operation(
        Policies const& policies,
        Scalar<policy::InputElementT<Policies, 0>> const& input_scalar,
        Functor const& functor) -> Scalar<policy::OutputElementT<Policies, 0>>
    {
        using OutputElement = policy::OutputElementT<Policies, 0>;

        return hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                [policies, functor](auto const& input_value) -> OutputElement
                {
                    auto const& dp = policies.domain_policy();
                    auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
                    auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                    OutputElement output_value;

                    if (indp.is_no_data(input_value) || !dp.within_domain(input_value))
                    {
                        ondp.mark_no_data(output_value);
                    }
                    else
                    {
                        output_value = functor(input_value);

                        if (!rp.within_range(input_value, output_value))
                        {
                            ondp.mark_no_data(output_value);
                        }
                    }

                    return output_value;
                }),
            input_scalar.future());
    }

}  // namespace lue


#define LUE_INSTANTIATE_UNARY_LOCAL_OPERATION(Policies, rank, Functor)                                       \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT auto                                                                 \
    unary_local_operation<ArgumentType<void(Policies)>, rank, ArgumentType<void(Functor)>>(                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<ArgumentType<void(Policies)>>, rank> const&,                  \
        ArgumentType<void(Functor)> const&)                                                                  \
        -> PartitionedArray<policy::OutputElementT<ArgumentType<void(Policies)>>, rank>;                     \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT auto                                                                 \
    unary_local_operation<ArgumentType<void(Policies)>, ArgumentType<void(Functor)>>(                        \
        ArgumentType<void(Policies)> const&,                                                                 \
        Scalar<policy::InputElementT<ArgumentType<void(Policies)>>> const&,                                  \
        ArgumentType<void(Functor)> const&) -> Scalar<policy::OutputElementT<ArgumentType<void(Policies)>>>;
