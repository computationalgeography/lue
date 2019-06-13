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

    // TODO Generalize. It is possible this code runs on a different
    //     partition than the one the partition passed in is located on.
    //     In that case the default copy doesn't work, does it? Implement
    //     the copy constructor.

    return Partition{partition};
}


template<
    typename Element,
    std::size_t rank>
struct CopyPartitionAction
{
};

}  // namespace detail
}  // namespace lue


#define LUE_COPY_PLAIN_ACTION(                                        \
    Element,                                                          \
    rank)                                                             \
                                                                      \
namespace lue {                                                       \
namespace detail {                                                    \
                                                                      \
decltype(auto) copy_partition_##Element##_##rank(                     \
    ArrayPartition<Element, rank> const& partition)                   \
{                                                                     \
    using Partition = ArrayPartition<Element, rank>;                  \
                                                                      \
    return copy_partition<Partition>(partition);                      \
}                                                                     \
                                                                      \
}                                                                     \
}                                                                     \
                                                                      \
                                                                      \
HPX_PLAIN_ACTION(                                                     \
    lue::detail::copy_partition_##Element##_##rank,                   \
    CopyPartitionAction_##Element##_##rank)                           \
                                                                      \
                                                                      \
namespace lue {                                                       \
namespace detail {                                                    \
                                                                      \
template<>                                                            \
class CopyPartitionAction<Element, rank>                              \
{                                                                     \
                                                                      \
public:                                                               \
                                                                      \
    using Type = CopyPartitionAction_##Element##_##rank;              \
                                                                      \
};                                                                    \
                                                                      \
}                                                                     \
}


#define LUE_COPY_PLAIN_ACTIONS(    \
    Element)                       \
                                   \
LUE_COPY_PLAIN_ACTION(Element, 1)  \
LUE_COPY_PLAIN_ACTION(Element, 2)


LUE_COPY_PLAIN_ACTIONS(/* std:: */ int32_t)
// LUE_COPY_PLAIN_ACTIONS(/* std:: */ int64_t)
// LUE_COPY_PLAIN_ACTIONS(float)
LUE_COPY_PLAIN_ACTIONS(double)


#undef LUE_COPY_PLAIN_ACTION
#undef LUE_COPY_PLAIN_ACTIONS


namespace lue {

template<
    typename Element,
    std::size_t rank>
using CopyPartitionActionType =
    typename detail::CopyPartitionAction<Element, rank>::Type;


template<
    typename Array>
Array copy(
    Array const& array)
{
    using Partition = PartitionT<Array>;
    using Partitions = PartitionsT<Array>;
    using Element = ElementT<Array>;

    using CopyPartitionAction = CopyPartitionActionType<Element, rank<Array>>;
    CopyPartitionAction copy_partition_action;

    // Create a new array for storing the new partitions. These partitions
    // will contain the copied elements.
    Partitions partitions{shape_in_partitions(array)};

    // Attach a continuation to each partition that copies all input
    // elements to the output elements. These continuations run on the
    // same localities as where the partitions themselves are located.
    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition const& source_partition = array.partitions()[p];
        Partition& destination_partition = partitions[p];

        destination_partition = hpx::dataflow(
            hpx::launch::async,
            copy_partition_action,
            hpx::get_colocation_id(
                hpx::launch::sync, source_partition.get_id()),
            source_partition
        );

    }

    return Array{shape(array), std::move(partitions)};
}

}  // namespace lue
