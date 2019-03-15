#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include <hpx/lcos/when_all.hpp>


namespace lue {

/*!
    @brief      Fill @a array in-place with @a fill_value
    @param      array Array to fill
    @param      fill_value Value to assign to each cell in @a array
    @return     Future that becomes ready once the algorithm has finished

    The existing @a array passed in is updated. Use the returned future if
    you need to know when the filling is done.
*/
template<
    typename Array
>
[[nodiscard]] hpx::future<void> fill(
    Array& array,
    hpx::shared_future<
        typename PartitionedArrayTypeTraits<Array>::ElementType>& fill_value)
{
    using ArrayTraits = PartitionedArrayTypeTraits<Array>;
    using Partition = typename ArrayTraits::PartitionType;
    using Element = typename ArrayTraits::ElementType;

    // Attach a continuation to each partition that fills all elements with
    // the value passed in
    std::vector<hpx::future<void>> fill_partitions(nr_partitions(array));

    using FillAction =
        typename ArrayPartitionFillAction<Element, rank(array)>::Type;
    FillAction action;

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition& partition = array.partitions()[p];

        fill_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(action),
            partition.get_id(),
            fill_value
        );

    }

    return hpx::when_all(fill_partitions);
}

}  // namespace lue
