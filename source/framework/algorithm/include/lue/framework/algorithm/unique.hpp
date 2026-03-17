#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    /*!
        @brief      Return the unique values in a partitioned array
        @tparam     Element Type of elements in the input array
        @tparam     rank Rank of the input array
        @tparam     Array Class template of the type of the array
        @param      input_array Partitioned array
        @return     Future to a set with the unique values that becomes
                    ready once the algorithm has finished
    */
    template<typename Policies, typename Element, Rank rank>
    auto unique(Policies const& policies, PartitionedArray<Element, rank> const& array)
        -> hpx::future<std::set<Element>>;

}  // namespace lue
