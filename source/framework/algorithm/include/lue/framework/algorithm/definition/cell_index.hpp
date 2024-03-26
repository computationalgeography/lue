#pragma once
#include "lue/framework/algorithm/cell_index.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::cell_index {

        template<typename Policies, typename ConditionPartition, typename IndexElement>
        auto cell_index_partition(
            Policies const& policies,
            ConditionPartition const& condition_partition,
            Index const dimension_idx) -> PartitionT<ConditionPartition, IndexElement>
        {
            // TODO Generalize for all ranks and all dimension_idx values. Use mdspan?
            static_assert(rank<ConditionPartition> == 2);

            using Offset = OffsetT<ConditionPartition>;
            using ConditionElement = ElementT<ConditionPartition>;
            using ConditionData = DataT<ConditionPartition>;
            using IndexPartition = PartitionT<ConditionPartition, IndexElement>;
            using IndexData = DataT<IndexPartition>;

            lue_hpx_assert(condition_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(
                    [policies, condition_partition, dimension_idx](
                        Offset const& offset, ConditionData const& condition_data)
                    {
                        HPX_UNUSED(condition_partition);

                        auto const partition_shape = condition_data.shape();
                        IndexData index_data{partition_shape};

                        auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        if (dimension_idx == 0)
                        {
                            Index idx{0};

                            for (Index idx0 = 0; idx0 < partition_shape[0]; ++idx0)
                            {
                                for (Index idx1 = 0; idx1 < partition_shape[1]; ++idx1)
                                {
                                    if (indp.is_no_data(condition_data, idx) ||
                                        condition_data[idx] == ConditionElement{0})
                                    {
                                        ondp.mark_no_data(index_data, idx);
                                    }
                                    else
                                    {
                                        if (condition_data[idx])
                                        {
                                            index_data[idx] = idx0;
                                        }
                                    }

                                    ++idx;
                                }
                            }
                        }
                        else if (dimension_idx == 1)
                        {
                            Index idx{0};

                            for (Index idx0 = 0; idx0 < partition_shape[0]; ++idx0)
                            {
                                for (Index idx1 = 0; idx1 < partition_shape[1]; ++idx1)
                                {
                                    if (indp.is_no_data(condition_data, idx) ||
                                        condition_data[idx] == ConditionElement{0})
                                    {
                                        ondp.mark_no_data(index_data, idx);
                                    }
                                    else
                                    {
                                        index_data[idx] = idx1;
                                    }

                                    ++idx;
                                }
                            }
                        }

                        return IndexPartition{hpx::find_here(), offset, std::move(index_data)};
                    }),
                condition_partition.offset(),
                condition_partition.data());
        }


        template<typename Policies, typename ConditionPartition, typename IndexElement>
        struct CellIndexPartitionAction:
            hpx::actions::make_action<
                decltype(&cell_index_partition<Policies, ConditionPartition, IndexElement>),
                &cell_index_partition<Policies, ConditionPartition, IndexElement>,
                CellIndexPartitionAction<Policies, ConditionPartition, IndexElement>>::type
        {
        };

    }  // namespace detail::cell_index


    template<typename Policies, Rank rank>
    auto cell_index(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& condition,
        Index const dimension_idx) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        using ConditionElement = policy::InputElementT<Policies, 0>;
        using ConditionArray = PartitionedArray<ConditionElement, rank>;
        using ConditionPartition = PartitionT<ConditionArray>;

        using IndexElement = policy::OutputElementT<Policies, 0>;
        using IndexArray = PartitionedArray<IndexElement, rank>;
        using IndexPartitions = PartitionsT<IndexArray>;

        static_assert(std::is_integral_v<ConditionElement>);
        static_assert(std::is_integral_v<IndexElement>);

        assert(dimension_idx < static_cast<Index>(rank));

        detail::cell_index::CellIndexPartitionAction<Policies, ConditionPartition, IndexElement> action;

        Localities<rank> const& localities{condition.localities()};
        auto const& condition_partitions{condition.partitions()};
        IndexPartitions index_partitions{condition_partitions.shape()};
        Count const nr_partitions{nr_elements(condition_partitions.shape())};

        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            index_partitions[partition_idx] = hpx::dataflow(
                hpx::launch::async,
                [locality_id = localities[partition_idx], action, policies, dimension_idx](
                    ConditionPartition const& condition_partition)
                { return action(locality_id, policies, condition_partition, dimension_idx); },
                condition_partitions[partition_idx]);
        }

        return {shape(condition), localities, std::move(index_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_CELL_INDEX(Policies, rank)                                                           \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>          \
    cell_index<ArgumentType<void(Policies)>, rank>(                                                          \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&,                                   \
        Index const);
