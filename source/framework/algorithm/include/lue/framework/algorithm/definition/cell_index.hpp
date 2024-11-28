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
            Index const dimension_idx,
            Index const idx_offset)
            -> std::tuple<PartitionT<ConditionPartition, IndexElement>, hpx::future<Count>>
        {
            // TODO Generalize for all ranks and all dimension_idx values. Use mdspan?
            static_assert(rank<ConditionPartition> == 2);

            static_assert(std::is_same_v<policy::InputElementT<Policies, 0>, ElementT<ConditionPartition>>);
            static_assert(std::is_same_v<policy::OutputElementT<Policies, 0>, IndexElement>);

            using Offset = OffsetT<ConditionPartition>;
            using ConditionElement = ElementT<ConditionPartition>;
            using ConditionData = DataT<ConditionPartition>;
            using IndexPartition = PartitionT<ConditionPartition, IndexElement>;
            using IndexData = DataT<IndexPartition>;

            lue_hpx_assert(condition_partition.is_ready());

            return hpx::split_future(hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(
                    [policies, condition_partition, dimension_idx, idx_offset](
                        Offset const& offset,
                        ConditionData const& condition_data) -> std::tuple<IndexPartition, Count>
                    {
                        HPX_UNUSED(condition_partition);

                        auto const partition_shape = condition_data.shape();
                        Count count{};
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
                                            index_data[idx] = idx_offset + idx0;
                                        }
                                    }

                                    ++idx;
                                }
                            }

                            count = std::get<1>(partition_shape);
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
                                        index_data[idx] = idx_offset + idx1;
                                    }

                                    ++idx;
                                }
                            }

                            count = std::get<0>(partition_shape);
                        }

                        IndexPartition partition{hpx::find_here(), offset, std::move(index_data)};

                        return {std::move(partition), count};
                    }),
                condition_partition.offset(),
                condition_partition.data()));
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


    /*!
        @brief      Return an array with for each valid true value in @a condition, the cell index along the
                    dimension @a dimension_idx
        @ingroup    array_operation

        All elements with a non-valid or false value in @a condition will become non-valid in the result.

        Cell indices don't vary along the dimension @a dimension_idx. They range from [0,
        array_shape[dimension_idx] - 1].
    */
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
        using IndexPartition = PartitionT<IndexArray>;
        using IndexPartitions = PartitionsT<IndexArray>;

        static_assert(std::is_integral_v<ConditionElement>);
        static_assert(std::is_integral_v<IndexElement>);

        assert(dimension_idx < static_cast<Index>(rank));

        detail::cell_index::CellIndexPartitionAction<Policies, ConditionPartition, IndexElement> action;

        Localities<rank> const& localities{condition.localities()};
        auto const& condition_partitions{condition.partitions()};
        IndexPartitions index_partitions{condition_partitions.shape()};

        // TODO Generalize for all ranks and all dimension_idx values. Use mdspan?
        static_assert(rank == 2);

        auto const shape_in_partitions = condition_partitions.shape();

        if (dimension_idx == 0)
        {
            // Spawn tasks for all partitions with idx 0 in the dimension at idx 0
            Index const partition_idx0{0};
            hpx::shared_future<Count> nr_elements0_f{};

            IndexPartition partition{};
            hpx::future<Count> nr_elements0_f_{};

            for (Index partition_idx1 = 0; partition_idx1 < std::get<1>(shape_in_partitions);
                 ++partition_idx1)
            {
                std::tie(partition, nr_elements0_f_) = hpx::split_future(hpx::dataflow(
                    hpx::launch::async,
                    [locality_id = localities(partition_idx0, partition_idx1),
                     action,
                     policies,
                     dimension_idx](ConditionPartition const& condition_partition)
                    {
                        Index const offset{0};

                        return action(locality_id, policies, condition_partition, dimension_idx, offset);
                    },
                    condition_partitions(partition_idx0, partition_idx1)));

                index_partitions(partition_idx0, partition_idx1) = std::move(partition);

                if (partition_idx1 == 0)
                {
                    nr_elements0_f = nr_elements0_f_.share();
                }
            }

            // Given that we now know how many cells are in a partition, we can spawn tasks for all other
            // partitions

            for (Index partition_idx0 = 1; partition_idx0 < std::get<0>(shape_in_partitions);
                 ++partition_idx0)
            {
                for (Index partition_idx1 = 0; partition_idx1 < std::get<1>(shape_in_partitions);
                     ++partition_idx1)
                {
                    std::tie(partition, std::ignore) = hpx::split_future(hpx::dataflow(
                        hpx::launch::async,
                        [locality_id = localities(partition_idx0, partition_idx1),
                         action,
                         policies,
                         dimension_idx,
                         partition_idx0](
                            ConditionPartition const& condition_partition,
                            hpx::shared_future<Count> const& nr_elements)
                        {
                            Index const offset{partition_idx0 * nr_elements.get()};

                            return action(locality_id, policies, condition_partition, dimension_idx, offset);
                        },
                        condition_partitions(partition_idx0, partition_idx1),
                        nr_elements0_f));

                    index_partitions(partition_idx0, partition_idx1) = std::move(partition);
                }
            }
        }
        else if (dimension_idx == 1)
        {
            // Spawn tasks for all partitions with idx 0 in the dimension at idx 1
            Index const partition_idx1{0};
            hpx::shared_future<Count> nr_elements1_f{};

            IndexPartition partition{};
            hpx::future<Count> nr_elements1_f_{};

            for (Index partition_idx0 = 0; partition_idx0 < std::get<0>(shape_in_partitions);
                 ++partition_idx0)
            {
                std::tie(partition, nr_elements1_f_) = hpx::split_future(hpx::dataflow(
                    hpx::launch::async,
                    [locality_id = localities(partition_idx0, partition_idx1),
                     action,
                     policies,
                     dimension_idx](ConditionPartition const& condition_partition)
                    {
                        Index const offset{0};

                        return action(locality_id, policies, condition_partition, dimension_idx, offset);
                    },
                    condition_partitions(partition_idx0, partition_idx1)));

                index_partitions(partition_idx0, partition_idx1) = std::move(partition);

                if (partition_idx0 == 0)
                {
                    nr_elements1_f = nr_elements1_f_.share();
                }
            }

            // Given that we now know how many cells are in a partition, we can spawn tasks for all other
            // partitions

            for (Index partition_idx0 = 0; partition_idx0 < std::get<0>(shape_in_partitions);
                 ++partition_idx0)
            {
                for (Index partition_idx1 = 1; partition_idx1 < std::get<1>(shape_in_partitions);
                     ++partition_idx1)
                {
                    std::tie(partition, std::ignore) = hpx::split_future(hpx::dataflow(
                        hpx::launch::async,
                        [locality_id = localities(partition_idx0, partition_idx1),
                         action,
                         policies,
                         dimension_idx,
                         partition_idx1](
                            ConditionPartition const& condition_partition,
                            hpx::shared_future<Count> const& nr_elements)
                        {
                            Index const offset{partition_idx1 * nr_elements.get()};

                            return action(locality_id, policies, condition_partition, dimension_idx, offset);
                        },
                        condition_partitions(partition_idx0, partition_idx1),
                        nr_elements1_f));

                    index_partitions(partition_idx0, partition_idx1) = std::move(partition);
                }
            }
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
