#pragma once
#include "lue/framework/partitioned_array_decl.hpp"


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
    template<typename Element, Rank rank>
    hpx::future<void> fill(
        PartitionedArray<Element, rank>& array, hpx::shared_future<Element> const& fill_value)
    {
        std::vector<hpx::future<void>> fill_partitions(nr_partitions(array));

        typename server::ArrayPartition<Element, rank>::FillAction action;

        for (Index p = 0; p < nr_partitions(array); ++p)
        {
            fill_partitions[p] =
                hpx::dataflow(hpx::launch::async, hpx::unwrapping(action), array.partitions()[p], fill_value);
        }

        return hpx::when_all(std::move(fill_partitions));
    }


    template<typename Element, Rank rank>
    hpx::future<void> fill(PartitionedArray<Element, rank>& array, Element const& fill_value)
    {
        return fill(array, hpx::make_ready_future<Element>(fill_value).share());
    }

}  // namespace lue
