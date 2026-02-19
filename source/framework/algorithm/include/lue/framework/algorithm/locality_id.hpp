#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/index_util.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, Rank rank>
        auto locality_id_partition(
            ArrayPartition<InputElement, rank> input_partition,
            Index const partition_idx,
            ShapeT<ArrayPartition<InputElement, rank>> const& shape_in_partitions)
            -> ArrayPartition<std::uint32_t, rank>
        {
            using Shape = ShapeT<ArrayPartition<InputElement, rank>>;
            auto partition_offset{linear_idx_to_idxs(partition_idx, shape_in_partitions)};

            return input_partition.then(

                [partition_offset]([[maybe_unused]] ArrayPartition<InputElement, rank> const& input_partition)
                {
                    Shape partition_shape{};
                    std::fill(partition_shape.begin(), partition_shape.end(), 1);

                    return ArrayPartition<std::uint32_t, rank>{
                        hpx::find_here(), partition_offset, partition_shape, hpx::get_locality_id()};
                }

            );
        }

    }  // namespace detail


    template<typename InputElement, Rank rank>
    struct LocalityIDPartitionAction:
        hpx::actions::make_action<
            decltype(&detail::locality_id_partition<InputElement, rank>),
            &detail::locality_id_partition<InputElement, rank>,
            LocalityIDPartitionAction<InputElement, rank>>::type
    {
    };


    template<typename InputElement, Rank rank>
    auto locality_id(PartitionedArray<InputElement, rank> const& input_array)
        -> PartitionedArray<std::uint32_t, rank>
    {
        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;

        using OutputArray = PartitionedArray<std::uint32_t, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        LocalityIDPartitionAction<InputElement, rank> action;

        Localities<rank> localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            output_partitions[p] = hpx::async(
                hpx::annotated_function(action, "locality_id"),
                localities[p],
                input_partitions[p],
                p,
                output_partitions.shape());
        }

        return OutputArray{
            shape_in_partitions(input_array), std::move(localities), std::move(output_partitions)};
    }

}  // namespace lue
