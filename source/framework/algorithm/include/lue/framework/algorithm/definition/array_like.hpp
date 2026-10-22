#pragma once
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/algorithm/miscellaneous_operation_export.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename Policies, Arithmetic InputElement, Arithmetic OutputElement, Rank rank>
        auto array_like_partition(
            Policies const& policies,
            ArrayPartition<InputElement, rank> const& input_partition,
            Scalar<OutputElement> const fill_value) -> ArrayPartition<OutputElement, rank>
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
                fill_value.future());
        }


        template<typename Policies, Arithmetic InputElement, Arithmetic OutputElement, Rank rank>
        struct ArrayLikePartitionAction:
            hpx::actions::make_action<
                decltype(&detail::array_like_partition<Policies, InputElement, OutputElement, rank>),
                &detail::array_like_partition<Policies, InputElement, OutputElement, rank>,
                ArrayLikePartitionAction<Policies, InputElement, OutputElement, rank>>::type
        {
        };

    }  // namespace detail


    template<Arithmetic OutputElement, typename Policies, Arithmetic InputElement, Rank rank>
    auto array_like(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& input_array,
        Scalar<OutputElement> const& fill_value) -> PartitionedArray<OutputElement, rank>
    {
        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;

        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        detail::ArrayLikePartitionAction<Policies, InputElement, OutputElement, rank> action{};

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index partition_idx = 0; partition_idx < nr_partitions(input_array); ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::async(
                action, localities[partition_idx], policies, input_partitions[partition_idx], fill_value);
        }

        return OutputArray{input_array, std::move(output_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_ARRAY_LIKE(Policies)                                                                 \
    template LUE_MISCELLANEOUS_OPERATION_EXPORT auto array_like<policy::OutputElementT<Policies, 0>>(        \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Scalar<policy::OutputElementT<Policies, 0>> const&)                                                  \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
