#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>
#include <numeric>


namespace lue {
namespace detail {

template<
    typename Partition>
[[nodiscard]] hpx::future<void> unique_id_partition(
    // TODO non-const ref is not possible for actions. Is this an issue
    //     in case of component client instances?
    Partition partition,
    ElementT<Partition> const start_value)
{
    // - Create a new collection of data elements
    // - Assign unique IDs to the cells
    // - Assign the collection of data elements
    // - Return a future that becomes ready once the data has been
    //     assigned to the existing partition

    using Data = DataT<Partition>;
    using Shape = ShapeT<Partition>;

    // Given the partition's shape, we can create a new collection for
    // partition elements
    return partition.shape().then(
        hpx::util::unwrapping(
            [partition, start_value](
                Shape const& shape) mutable
            {
                Data data{shape};
                std::iota(data.begin(), data.end(), start_value);

                // This runs asynchronous and returns a future<void>
                return partition.set_data(std::move(data)); // data, CopyMode::share);
            }));

    // auto assigned = data.then(
    //     hpx::util::unwrapping(
    //         [start_value](Data& partition_data)
    //         {
    //             std::iota(
    //                 partition_data.begin(), partition_data.end(), start_value);
    //         }));

    // return assigned.then(
    //     hpx::util::unwrapping(
    //         [partition](Data& assigned_data)
    //         {
    //             partition.set_data(assigned_data);
    //         }));
}


template<
    typename Element,
    std::size_t rank>
struct UniqueIDPartitionAction
{
};

}  // namespace detail


template<
    typename Partition>
struct UniqueIDPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::unique_id_partition<Partition>),
        &detail::unique_id_partition<Partition>,
        UniqueIDPartitionAction<Partition>>
{};


template<
    typename Array>
[[nodiscard]] hpx::future<void> unique_id(
    Array& array)
{
    // - Assign a unique value to each cell in the array

    // - Iterate over all partitions
    //     - Determine first value for partition
    //     - Call action that fills the partition with unique IDs
    // - Return future that becomes ready once all partitions are done
    //     assigning values to all cells

    using Element = ElementT<Array>;
    using Partition = PartitionT<Array>;

    std::vector<hpx::future<void>> unique_id_partitions(nr_partitions(array));

    UniqueIDPartitionAction<Partition> action;

    Element start_value = 0;

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {
        Partition& partition = array.partitions()[p];

        unique_id_partitions[p] = hpx::dataflow(
            hpx::launch::async, action,
            // TODO Is this efficient?
            hpx::get_colocation_id(hpx::launch::sync, partition.get_id()),
            partition, start_value);

        start_value += partition.size().get();  // TODO blocks
    }

    return hpx::when_all(unique_id_partitions);
}

}  // namespace lue
