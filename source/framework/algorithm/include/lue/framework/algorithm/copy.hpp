#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return a new copy the array partition passed in
    @tparam     Partition Client class of partition component
    @param      partition Client of array partition component
    @return     A copy of the array partition passed in
*/
template<
    typename Partition>
Partition copy_partition(
    Partition const& partition)
{
    // Assert the locality of the partition is the same as the locality
    // this code runs on
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    // Does a copy of a component create a copy on the locality the
    // original component is located on? In that case, this code
    // seems fine.

    // Deep copy
    return Partition{partition};
}

}  // namespace detail


template<
    typename Partition>
struct CopyPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::copy_partition<Partition>),
        &detail::copy_partition<Partition>,
        CopyPartitionAction<Partition>>
{};


template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array_>
Array_<Element, rank> copy(
    Array_<Element, rank> const& array)
{
    using Array = Array_<Element, rank>;
    using Partition = PartitionT<Array>;
    using Partitions = PartitionsT<Array>;

    CopyPartitionAction<Partition> action;
    Partitions output_partitions{shape_in_partitions(array)};

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition const& input_partition = array.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::naming::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            input_partition);
                    }
                )
            );
    }

    return Array{shape(array), std::move(output_partitions)};
}

}  // namespace lue
