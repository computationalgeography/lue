#pragma once
#include "lue/framework/algorithm/detail/partition.hpp"
#include "lue/framework/algorithm/detail/promise.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/unique_id.hpp"
#include "lue/macro.hpp"
#include <hpx/async_combinators/when_any.hpp>
#include <vector>


namespace lue {
    namespace detail {

        template<typename Policies, typename ConditionPartition, typename IDPartition>
        auto unique_id_partition(
            Policies const& policies,
            ConditionPartition const& condition_partition,
            ElementT<IDPartition> start_value) -> IDPartition
        {
            lue_hpx_assert(condition_partition.is_ready());
            lue_hpx_assert(start_value > ElementT<IDPartition>{0});

            using Offset = OffsetT<ConditionPartition>;
            using ConditionData = DataT<ConditionPartition>;

            using IDData = DataT<IDPartition>;

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [condition_partition, policies, start_value](
                        Offset const& offset,
                        ConditionData const& input_partition_data) mutable  // The promise is set
                    {
                        HPX_UNUSED(condition_partition);

                        IDData output_partition_data{input_partition_data.shape()};

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
                                if (input_partition_data[i])
                                {
                                    output_partition_data[i] = start_value++;
                                }
                                else
                                {
                                    output_partition_data[i] = 0;
                                }
                            }
                        }

                        return IDPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                    ),
                condition_partition.offset(),
                condition_partition.data());
        }


        template<typename Policies, typename ConditionPartition, typename IDPartition>
        struct UniqueIDPartitionAction:
            hpx::actions::make_action<
                decltype(&unique_id_partition<Policies, ConditionPartition, IDPartition>),
                &unique_id_partition<Policies, ConditionPartition, IDPartition>,
                UniqueIDPartitionAction<Policies, ConditionPartition, IDPartition>>::type
        {
        };


        template<typename IDElement, typename Policies, typename ConditionElement, Rank rank>
        void unique_id_partitions2(
            Policies const& policies,
            Localities<rank> const& localities,
            PartitionsT<PartitionedArray<ConditionElement, rank>> const& condition_partitions,
            Array<hpx::future<Count>, rank>&& partition_sizes,
            IDPromiseArray<rank> id_partitions_component_ids)
        {
            // Given the collection of input partitions and their sizes:
            // - Wait for any partition size to be returned / set. Spawn a task that will create
            //   a result partition given 1) the input partition and 2) the current start value for
            //   unique IDs to generate.
            // - Assign the resulting partition to value of the promise associated with the
            //   current partition
            // - Update the start value for unique IDs with the partition size just obtained.

            Count const nr_partitions{condition_partitions.nr_elements()};
            Count nr_partitions_to_handle{nr_partitions};

            lue_hpx_assert(nr_elements(partition_sizes) == nr_partitions);

            hpx::when_any_result<std::vector<hpx::future<Count>>> when_any_result;
            std::size_t remaining_partition_idx{};

            IDElement start_value{1};
            Count nr_elements{};

            using ConditionArray = PartitionedArray<ConditionElement, rank>;
            using ConditionPartition = PartitionT<ConditionArray>;

            using IDArray = PartitionedArray<IDElement, rank>;
            using IDPartition = PartitionT<IDArray>;

            detail::UniqueIDPartitionAction<Policies, ConditionPartition, IDPartition> action{};

            // Create collection of linear partition indices. When spawning tasks for partitions,
            // we will reorder these linear partitions, depending on which partitions are handled
            // first. This collection is used to map indices to partitions that being handled to
            // original indices in the original collections.
            std::vector<Index> partition_idxs(nr_partitions);
            std::iota(partition_idxs.begin(), partition_idxs.end(), 0);
            std::size_t partition_idx{};

            // Linear collection of futures to partition sizes
            std::vector<hpx::future<Count>> partition_sizes_(nr_partitions);
            std::move(partition_sizes.begin(), partition_sizes.end(), partition_sizes_.begin());

            while (nr_partitions_to_handle > 0)
            {
                when_any_result =
                    hpx::when_any(
                        partition_sizes_.begin(), partition_sizes_.begin() + nr_partitions_to_handle)
                        .get();

                partition_sizes_ = std::move(when_any_result.futures);
                remaining_partition_idx = when_any_result.index;
                partition_idx = partition_idxs[remaining_partition_idx];
                nr_elements = partition_sizes_[remaining_partition_idx].get();

                hpx::async(
                    action,
                    localities[partition_idx],
                    policies,
                    condition_partitions[partition_idx],
                    start_value)
                    .then(

                        [promise = std::move(id_partitions_component_ids[partition_idx])](
                            auto&& partition) mutable { promise.set_value(partition.get().get_id()); }

                    );

                start_value += nr_elements;

                // Move the partition stuff just handled to just after the range with still to
                // handle partition stuff
                std::rotate(
                    partition_sizes_.begin() + remaining_partition_idx,
                    partition_sizes_.begin() + remaining_partition_idx + 1,
                    partition_sizes_.begin() + nr_partitions_to_handle);

                std::rotate(
                    partition_idxs.begin() + remaining_partition_idx,
                    partition_idxs.begin() + remaining_partition_idx + 1,
                    partition_idxs.begin() + nr_partitions_to_handle);

                --nr_partitions_to_handle;
            }
        }


        template<typename IDElement, typename Policies, typename ConditionElement, Rank rank>
        auto unique_id_partitions(
            Policies const& policies,
            Localities<rank> const& localities,
            PartitionsT<PartitionedArray<ConditionElement, rank>> const& condition_partitions,
            Array<hpx::future<Count>, rank>&& partition_sizes)
            -> PartitionsT<PartitionedArray<IDElement, rank>>
        {
            using IDArray = PartitionedArray<IDElement, rank>;
            using IDPartitions = PartitionsT<IDArray>;
            using IDPartition = PartitionT<IDPartitions>;

            using Shape = ShapeT<IDPartitions>;

            Shape const& shape_in_partitions{condition_partitions.shape()};

            // For each partition a promise of a partition server instance ID
            IDPromiseArray<rank> id_partitions_component_ids{shape_in_partitions};

            // For each partition a client instance, that contains a future to a partition
            // server instance ID
            IDPartitions id_partitions{shape_in_partitions};

            Count const nr_partitions{nr_elements(shape_in_partitions)};

            for (Index p = 0; p < nr_partitions; ++p)
            {
                // Connect the array with promises with the array of futures
                id_partitions[p] = IDPartition{id_partitions_component_ids[p].get_future()};
                lue_hpx_assert(id_partitions[p].valid());
            }

            // Asynchronously plan work that will result in result partitions being filled with
            // unique IDs. While this work is being executed, the promises of the individual
            // partition server instance IDs will be fulfilled, as soon as possible. Fire
            // and forget.
            hpx::post(
                unique_id_partitions2<IDElement, Policies, ConditionElement, rank>,
                policies,
                localities,
                condition_partitions,
                std::forward<Array<hpx::future<Count>, rank>>(partition_sizes),
                std::move(id_partitions_component_ids));

            return id_partitions;
        }

    }  // namespace detail


    template<typename IDElement, typename Policies, typename ConditionElement, Rank rank>
    auto unique_id(Policies const& policies, PartitionedArray<ConditionElement, rank> const& condition_array)
        -> PartitionedArray<IDElement, rank>
    {
        using ConditionArray = PartitionedArray<ConditionElement, rank>;
        using ConditionPartitions = PartitionsT<ConditionArray>;

        using IDArray = PartitionedArray<IDElement, rank>;
        using IDPartitions = PartitionsT<IDArray>;

        ConditionPartitions const& input_partitions{condition_array.partitions()};

        // For each partition, a future to its size
        Array<hpx::future<Count>, rank> partition_sizes{detail::partition_sizes(input_partitions)};

        // Requirements for what happens next:
        // - We need to return an array ASAP
        // - We also need to wait for partition sizes to get returned
        // - We have to return an array, not a future to an array
        // - We don't need to create result partitions in some order. Just spawn work once
        //   one of the partition sizes gets returned.
        //
        // Solution:
        // 1. Spawn a task that will return array partitions immediately
        // 2. In the task, process each input partition as soon as its size is known. It is
        //    ready by then. Assign the result partition as a value of the promise associated
        //    with the partition that has already been returned (the partition client).

        IDPartitions id_partitions{detail::unique_id_partitions<IDElement, Policies, ConditionElement, rank>(
            policies,
            condition_array.localities(),
            condition_array.partitions(),
            std::move(partition_sizes))};

        return IDArray{shape(condition_array), condition_array.localities(), std::move(id_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_UNIQUE_ID(Policies, IDElement, ConditionElement, rank)                               \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<IDElement, rank>                                    \
    unique_id<IDElement, ArgumentType<void(Policies)>, ConditionElement, rank>(                              \
        ArgumentType<void(Policies)> const&, PartitionedArray<ConditionElement, rank> const&);
