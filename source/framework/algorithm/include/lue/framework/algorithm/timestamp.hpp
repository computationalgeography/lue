#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/index_util.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include <chrono>


namespace lue {

    using ClockTick = std::chrono::high_resolution_clock::duration::rep;


    namespace detail {

        template<typename InputElement, Rank rank>
        ArrayPartition<ClockTick, 2> timestamp_action(
            ArrayPartition<InputElement, rank> input_partition,
            Index const partition_idx,
            ShapeT<ArrayPartition<InputElement, rank>> const& shape_in_partitions)
        {
            auto partition_offset{linear_idx_to_idxs(partition_idx, shape_in_partitions)};

            return input_partition.then(

                [partition_offset]([[maybe_unused]] ArrayPartition<InputElement, rank> const& input_partition)
                {
                    return ArrayPartition<ClockTick, rank>{
                        hpx::find_here(),
                        partition_offset,
                        {1, 1},
                        std::chrono::high_resolution_clock{}.now().time_since_epoch().count()};
                }

            );
        }


        template<typename InputElement, Rank rank>
        struct TimestampAction:
            hpx::actions::make_action<
                decltype(&timestamp_action<InputElement, rank>),
                &timestamp_action<InputElement, rank>,
                TimestampAction<InputElement, rank>>::type
        {
        };

    }  // namespace detail


    template<typename InputElement, Rank rank>
    auto timestamp(PartitionedArray<InputElement, rank> const& input_array)
        -> PartitionedArray<ClockTick, rank>
    {
        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;

        using TimestampArray = PartitionedArray<ClockTick, rank>;
        using TimestampPartitions = PartitionsT<TimestampArray>;

        detail::TimestampAction<InputElement, rank> action{};

        Localities<rank> localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        Count const nr_partitions{lue::nr_partitions(input_array)};
        TimestampPartitions timestamp_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions; ++p)
        {
            timestamp_partitions[p] = hpx::async(
                hpx::annotated_function(action, "timestamp"),
                localities[p],
                input_partitions[p],
                p,
                timestamp_partitions.shape());
        }

        return TimestampArray{
            shape_in_partitions(input_array), std::move(localities), std::move(timestamp_partitions)};
    }

}  // namespace lue
