#pragma once
#include "lue/framework/algorithm/global_operation_export.hpp"
#include "lue/framework/algorithm/reclassify.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename Policies, typename InputPartition, typename OutputPartition>
        auto reclassify_partition(
            Policies const& policies,
            InputPartition const& input_partition,
            hpx::shared_future<LookupTable<ElementT<InputPartition>, ElementT<OutputPartition>>> const&
                lookup_table) -> OutputPartition
        {
            using Offset = OffsetT<InputPartition>;
            using InputData = DataT<InputPartition>;
            using FromElement = ElementT<InputPartition>;
            using ToElement = ElementT<OutputPartition>;
            using OutputData = DataT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,

                [policies](
                    InputPartition const& input_partition,
                    hpx::shared_future<LookupTable<FromElement, ToElement>> const& lookup_table_f)
                    -> OutputPartition
                {
                    AnnotateFunction const annotation{"reclassify partition"};

                    Offset const offset = input_partition.offset(hpx::launch::sync);
                    InputData const input_partition_data = input_partition.data(hpx::launch::sync);
                    auto const& lookup_table = lookup_table_f.get();
                    OutputData output_partition_data{input_partition_data.shape()};

                    auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                    Count const nr_elements{lue::nr_elements(input_partition_data)};

                    for (Index idx = 0; idx < nr_elements; ++idx)
                    {
                        if (indp.is_no_data(input_partition_data, idx))
                        {
                            ondp.mark_no_data(output_partition_data, idx);
                        }
                        else
                        {
                            FromElement const from_value{input_partition_data[idx]};

                            auto const it{lookup_table.find(from_value)};

                            if (it == lookup_table.end())
                            {
                                ondp.mark_no_data(output_partition_data, idx);
                            }
                            else
                            {
                                output_partition_data[idx] = (*it).second;
                            }
                        }
                    }

                    return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                },

                input_partition,
                lookup_table);
        }


        template<typename Policies, typename InputPartition, typename OutputPartition>
        struct ReclassifyPartitionAction:
            hpx::actions::make_action<
                decltype(&reclassify_partition<Policies, InputPartition, OutputPartition>),
                &reclassify_partition<Policies, InputPartition, OutputPartition>,
                ReclassifyPartitionAction<Policies, InputPartition, OutputPartition>>::type
        {
        };

    }  // namespace detail


    template<typename Policies, typename FromElement, typename ToElement, Rank rank>
    auto reclassify(
        Policies const& policies,
        PartitionedArray<FromElement, rank> const& input_array,
        hpx::shared_future<LookupTable<FromElement, ToElement>> const& lookup_table)
        -> PartitionedArray<ToElement, rank>
    {
        // Spawn a task for each partition that will reclassify it

        static_assert(std::is_integral_v<FromElement>);

        using InputArray = PartitionedArray<FromElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<ToElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        detail::ReclassifyPartitionAction<Policies, InputPartition, OutputPartition> action;

        Localities<rank> localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index partition_idx = 0; partition_idx < nr_partitions(input_array); ++partition_idx)
        {
            output_partitions[partition_idx] = hpx::async(
                action, localities[partition_idx], policies, input_partitions[partition_idx], lookup_table);
        }

        return OutputArray{shape(input_array), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_RECLASSIFY(Policies, FromElement, ToElement, rank)                                   \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT PartitionedArray<ToElement, rank>                                   \
    reclassify<ArgumentType<void(Policies)>, FromElement, ToElement, rank>(                                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<FromElement, rank> const&,                                                          \
        hpx::shared_future<LookupTable<FromElement, ToElement>> const&);
