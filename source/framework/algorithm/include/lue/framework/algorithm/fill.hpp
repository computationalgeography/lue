#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {

/*!
    @brief      Fill a partitioned array in-place with @a fill_value
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input arrays
    @tparam     Array Class template of the type of the arrays
    @param      array Partitioned array
    @param      fill_value Value to assign to each cell in @a array
    @return     Future that becomes ready once the algorithm has finished

    The existing @a array passed in is updated. Use the returned future if
    you need to know when the filling is done.
*/
template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
[[nodiscard]] hpx::future<void> fill(
    Array<Element, rank>& array,
    hpx::shared_future<Element>& fill_value)
{
    using Array_ = Array<Element, rank>;
    using Partition = PartitionT<Array_>;

    std::vector<hpx::future<void>> fill_partitions(nr_partitions(array));

    typename ArrayPartitionFillAction<Element, rank>::Type action;

    for(std::size_t p = 0; p < nr_partitions(array); ++p) {

        Partition& partition = array.partitions()[p];

        fill_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(action),
            partition.get_id(),
            fill_value
        );

    }

    // when_all takes an r-value reference. Why don't we have to move
    // the vector of futures in here?
    return hpx::when_all(fill_partitions);
}

}  // namespace lue
