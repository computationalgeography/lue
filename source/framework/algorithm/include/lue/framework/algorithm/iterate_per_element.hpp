#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

template<
    typename Partition>
Partition iterate_per_element_partition(
    Partition const& partition)
{
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using Element = ElementT<Partition>;
    using InputPartition = Partition;
    using InputData = DataT<InputPartition>;

    // Once the data of the input partition has arrived, do whatever it
    // takes to create the output partition
    return partition.data(CopyMode::share).then(
        hpx::launch::sync,  // Continue on current thread
        hpx::util::unwrapping(
            [](
                InputData const& partition_data)
            {
                // The use of volatile prevends the optimizing compiler
                // to remove this iteration
                for(volatile Element nr_iterations: partition_data) {
                    assert(nr_iterations >= Element{0});

                    while(nr_iterations > Element{0}) {
                        --nr_iterations;
                    }
                }

                // Copy the data and move it into a new partition
                return Partition{hpx::find_here(), InputData{partition_data}};
            }
        )
    );
}

}  // namespace detail


template<
    typename Partition>
struct IteratePerElementPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::iterate_per_element_partition<Partition>),
        &detail::iterate_per_element_partition<Partition>,
        IteratePerElementPartitionAction<Partition>>
{};


/*!
    @brief      Per cell in a partitioned array, iterate a number of
                times before copying the cell to the result
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the arrays
    @param      array Partitioned array
    @return     New partitioned array

    This algorithm has the same effect as the copy algorithm, but spends
    some more time. It is useful when simulating a configurable amount
    of load per cell.
*/
template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
Array<Element, rank> iterate_per_element(
    Array<Element, rank> const& array)
{
    using Array_ = Array<Element, rank>;
    using Partition = PartitionT<Array_>;
    using Partitions = PartitionsT<Array_>;

    IteratePerElementPartitionAction<Partition> action;
    Partitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition const& input_partition = array.partitions()[p];

        // Asynchronously determine the contents of the new partition
        // - First determine the locality the input partition is located on
        // - Then *call the action on that locality*, passing in the
        //     input partition
        output_partitions[p] =
            hpx::get_colocation_id(input_partition.get_id()).then(
                hpx::launch::sync,  // Continue on current thread
                hpx::util::unwrapping(
                   [action, input_partition](
                       hpx::id_type const locality_id)
                   {
                       // Detach from current thread
                       return hpx::dataflow(
                           hpx::launch::async,
                           action,
                           locality_id,
                           input_partition);
                   }
                )
            );

    }

    return Array_{shape(array), std::move(output_partitions)};
}

}  // namespace lue
