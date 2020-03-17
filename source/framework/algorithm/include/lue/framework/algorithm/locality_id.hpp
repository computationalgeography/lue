#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename Partition>
PartitionT<Partition, std::uint32_t> locality_id_partition(
    Partition const& input_partition)
{
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    using OutputPartition = PartitionT<Partition, std::uint32_t>;

    auto const input_partition_server_ptr{
        hpx::get_ptr(hpx::launch::sync, input_partition)};
    auto const& input_partition_server{*input_partition_server_ptr};

    std::uint32_t const locality_id{hpx::get_locality_id()};

    return OutputPartition{
        hpx::find_here(),
        input_partition_server.offset(),
        input_partition_server.shape(),
        locality_id};
}

}  // namespace detail


template<
    typename Partition>
struct LocalityIDPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::locality_id_partition<Partition>),
        &detail::locality_id_partition<Partition>,
        LocalityIDPartitionAction<Partition>>
{};


template<
    typename InputArray>
PartitionedArrayT<InputArray, std::uint32_t> locality_id(
    InputArray const& input_array)
{
    // Iterate over all partitions and spawn a task per partition. Each
    // of these tasks runs on the locality the partition is located on and
    // creates a new partition with the same shape as the input partition.
    // Each cell in the new partition is assigned the locality-id of
    // the locality the partition is located on.

    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArrayT<InputArray, std::uint32_t>;
    using OutputPartitions = PartitionsT<OutputArray>;

    LocalityIDPartitionAction<InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    for(Index p = 0; p < nr_partitions(input_array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

                [action](
                    InputPartition const& input_partition,
                    hpx::future<hpx::id_type>&& locality_id)
                {
                    return action(
                        locality_id.get(),
                        input_partition);
                },

            input_array.partitions()[p],
            hpx::get_colocation_id(input_array.partitions()[p].get_id()));

    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
}

}  // namespace lue
