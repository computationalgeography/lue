#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

/*!
    @brief      Return a new copy the array partition passed in
    @tparam     Partition Client class of partition component
    @param      input_partition Client of array partition component
    @return     A copy of the array partition passed in
*/
template<
    typename Partition>
Partition copy_partition(
    Partition const& input_partition)
{
    // Assert the locality of the partition is the same as the locality
    // this code runs on
    assert(
        hpx::get_colocation_id(input_partition.get_id()).get() ==
        hpx::find_here());

    // Implement copy constructor that does a deep copy?
    // return Partition{input_partition};

    // Copy the data and move it into a new partition

    // FIXME Make asynchronous
    auto offset = input_partition.offset().get();
    auto input_partition_data = input_partition.data(CopyMode::copy).get();

    return Partition{
        hpx::find_here(), offset, std::move(input_partition_data)};
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


/*!
    @brief      Return the result of copying a partitioned array
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input array
    @tparam     Array Class template of the type of the arrays
    @param      array Partitioned array
    @return     New partitioned array
*/
template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
Array<Element, rank> copy(
    Array<Element, rank> const& array)
{
    using Array_ = Array<Element, rank>;
    using Partition = PartitionT<Array_>;
    using Partitions = PartitionsT<Array_>;

    CopyPartitionAction<Partition> action;
    Partitions output_partitions{
        shape_in_partitions(array), scattered_target_index()};

    for(Index p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,

            [action](
                Partition const& input_partition)
            {
                return action(
                    // FIXME
                    hpx::get_colocation_id(
                        hpx::launch::sync, input_partition.get_id()),
                    input_partition);
            },

            array.partitions()[p]);

    }

    return Array_{shape(array), std::move(output_partitions)};
}

}  // namespace lue
