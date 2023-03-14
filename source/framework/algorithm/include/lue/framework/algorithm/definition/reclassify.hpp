#pragma once
#include "lue/framework/algorithm/global_operation_export.hpp"
#include "lue/framework/algorithm/reclassify.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename OutputPartition, typename Policies, typename InputPartition>
        OutputPartition reclassify_partition_ready(
            Policies const& policies,
            InputPartition const& input_partition,
            LookupTable<ElementT<InputPartition>, ElementT<OutputPartition>> const& lookup_table)
        {
            using Offset = OffsetT<InputPartition>;
            using InputData = DataT<InputPartition>;
            using FromElement = ElementT<InputPartition>;

            using OutputData = DataT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [input_partition, policies, lookup_table](
                        Offset const& offset, InputData const& input_partition_data)
                    {
                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{input_partition_data.shape()};

                        auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        Count const nr_elements{lue::nr_elements(input_partition_data)};

                        for (Index i = 0; i < nr_elements; ++i)
                        {
                            if (indp.is_no_data(input_partition_data, i))
                            {
                                ondp.mark_no_data(output_partition_data, i);
                            }
                            else
                            {
                                FromElement const from_value{input_partition_data[i]};

                                auto const it{lookup_table.find(from_value)};

                                if (it == lookup_table.end())
                                {
                                    ondp.mark_no_data(output_partition_data, i);
                                }
                                else
                                {
                                    output_partition_data[i] = (*it).second;
                                }
                            }
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                    ),
                input_partition.offset(),
                input_partition.data());
        }


        template<typename Policies, typename InputPartition, typename OutputPartition>
        OutputPartition reclassify_partition(
            Policies const& policies,
            InputPartition const& input_partition,
            hpx::shared_future<LookupTable<ElementT<InputPartition>, ElementT<OutputPartition>>> const&
                lookup_table)
        {
            using FromElement = ElementT<InputPartition>;
            using ToElement = ElementT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,

                [policies](
                    InputPartition const& input_partition,
                    hpx::shared_future<LookupTable<FromElement, ToElement>> const& lookup_table) {
                    return reclassify_partition_ready<OutputPartition>(
                        policies, input_partition, lookup_table.get());
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
    PartitionedArray<ToElement, rank> reclassify(
        Policies const& policies,
        PartitionedArray<FromElement, rank> const& input_array,
        hpx::shared_future<LookupTable<FromElement, ToElement>> const& lookup_table)
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

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            output_partitions[p] =
                hpx::async(action, localities[p], policies, input_partitions[p], lookup_table);
        }

        return OutputArray{shape(input_array), localities, std::move(output_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_RECLASSIFY(Policies, FromElement, ToElement, rank)                                   \
                                                                                                             \
    template LUE_GLOBAL_OPERATION_EXPORT PartitionedArray<ToElement, rank>                                   \
    reclassify<ArgumentType<void(Policies)>, FromElement, ToElement, rank>(                                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<FromElement, rank> const&,                                                          \
        hpx::shared_future<LookupTable<FromElement, ToElement>> const&);
