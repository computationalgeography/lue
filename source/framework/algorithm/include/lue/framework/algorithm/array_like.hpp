#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace detail {

        template<typename Policies, typename InputElement, typename OutputElement, Rank rank>
        auto array_like_partition(
            Policies const& policies,
            ArrayPartition<InputElement, rank> const& input_partition,
            hpx::shared_future<OutputElement> const fill_value) -> ArrayPartition<OutputElement, rank>
        {
            using InputPartition = ArrayPartition<InputElement, rank>;
            using Offset = OffsetT<InputPartition>;
            using Shape = ShapeT<InputPartition>;

            using OutputPartition = ArrayPartition<OutputElement, rank>;
            using OutputData = DataT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,

                [policies](
                    InputPartition const& input_partition,
                    hpx::shared_future<OutputElement> const& fill_value_f) -> auto
                {
                    AnnotateFunction annotation{"array_like_partition"};

                    Offset const offset = input_partition.offset(hpx::launch::sync);
                    Shape const shape = input_partition.shape(hpx::launch::sync);
                    OutputElement const fill_value = fill_value_f.get();
                    OutputData output_partition_data{shape};

                    // NOTE Can't use indp to check for no-data in the (output!) fill_value
                    // auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    // auto const& ondp =
                    // std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    Count const nr_elements{lue::nr_elements(output_partition_data)};

                    // if (indp.is_no_data(fill_value))
                    // {
                    //     for (Index i = 0; i < nr_elements; ++i)
                    //     {
                    //         ondp.mark_no_data(output_partition_data, i);
                    //     }
                    // }
                    // else
                    // {
                    for (Index i = 0; i < nr_elements; ++i)
                    {
                        output_partition_data[i] = fill_value;
                    }
                    // }

                    return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                },

                input_partition,
                fill_value);
        }

    }  // namespace detail


    namespace policy::array_like {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::array_like


    template<typename Policies, typename InputElement, typename OutputElement, Rank rank>
    struct ArrayLikePartitionAction:
        hpx::actions::make_action<
            decltype(&detail::array_like_partition<Policies, InputElement, OutputElement, rank>),
            &detail::array_like_partition<Policies, InputElement, OutputElement, rank>,
            ArrayLikePartitionAction<Policies, InputElement, OutputElement, rank>>::type
    {
    };


    template<typename OutputElement, typename Policies, typename InputElement, Rank rank>
    auto array_like(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        hpx::shared_future<OutputElement> const& fill_value) -> PartitionedArray<OutputElement, rank>
    {
        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;

        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        ArrayLikePartitionAction<Policies, InputElement, OutputElement, rank> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index partition_idx = 0; partition_idx < nr_partitions(input_array); ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::async(
                action, localities[partition_idx], policies, input_partitions[partition_idx], fill_value);
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }


    template<typename OutputElement, typename Policies, typename InputElement, Rank rank>
    auto array_like(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        OutputElement const fill_value) -> PartitionedArray<OutputElement, rank>
    {
        return array_like(policies, input_array, hpx::make_ready_future<OutputElement>(fill_value).share());
    }


    template<typename OutputElement, typename InputElement, Rank rank>
    auto array_like(PartitionedArray<InputElement, rank> const& input_array, OutputElement const fill_value)
        -> PartitionedArray<OutputElement, rank>
    {
        using Policies = policy::array_like::DefaultPolicies<OutputElement, InputElement>;

        return array_like(Policies{}, input_array, fill_value);
    }

}  // namespace lue
