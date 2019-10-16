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

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

            [](
                hpx::id_type const locality_id,
                InputData&& partition_data)
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
                return Partition{locality_id, InputData{partition_data}};
            }

        ),
        hpx::get_colocation_id(partition.get_id()),
        partition.data(CopyMode::share));
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

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                [action](
                    hpx::id_type const component_id)
                {
                    return action(
                        hpx::get_colocation_id(
                            hpx::launch::sync, component_id),
                        Partition{component_id});
                }

            ),
            array.partitions()[p]);

    }

    return Array_{shape(array), std::move(output_partitions)};
}

}  // namespace lue
