#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
ArrayPartition<OutputElement, rank> array_like_partition(
    ArrayPartition<InputElement, rank> const& input_partition,
    OutputElement const fill_value)
{
    using OutputPartition = ArrayPartition<OutputElement, rank>;
    using OutputPartitionData = DataT<OutputPartition>;

    // Assert the locality of the partition is the same as the locality
    // this code runs on
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    // Copy the data and move it into a new partition
    auto const input_partition_server_ptr{hpx::get_ptr(input_partition).get()};
    auto const& input_partition_server{*input_partition_server_ptr};

    auto offset{input_partition_server.offset()};
    auto shape{input_partition_server.shape()};
    auto target_idx{input_partition_server.target_idx()};

    return OutputPartition{
        hpx::find_here(), offset,
        OutputPartitionData{shape, fill_value, target_idx}};
}

}  // namespace detail


template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
struct ArrayLikePartitionAction:
    hpx::actions::make_action<
        decltype(&detail::array_like_partition<
            InputElement, OutputElement, rank>),
        &detail::array_like_partition<
            InputElement, OutputElement, rank>,
        ArrayLikePartitionAction<
            InputElement, OutputElement, rank>>
{};


template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> array_like(
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<OutputElement> const& fill_value)
{
    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    ArrayLikePartitionAction<InputElement, OutputElement, rank> action;
    OutputPartitions output_partitions{
        shape_in_partitions(input_array), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                InputPartition const& input_partition,
                hpx::shared_future<OutputElement> const& fill_value,
                hpx::future<hpx::id_type>&& locality_id)
            {
                return action(
                    locality_id.get(),
                    input_partition,
                    fill_value.get());
            },

            input_array.partitions()[p],
            fill_value,
            hpx::get_colocation_id(input_array.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}


template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
PartitionedArray<OutputElement, rank> array_like(
    PartitionedArray<InputElement, rank> const& input_array,
    OutputElement const fill_value)
{
    return array_like(
        input_array,
        hpx::make_ready_future<OutputElement>(fill_value).share());
}

}  // namespace lue
