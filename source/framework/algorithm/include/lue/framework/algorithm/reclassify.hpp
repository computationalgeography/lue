#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include <unordered_map>


namespace lue {

    template<typename FromElement, typename ToElement>
    using LookupTable = std::unordered_map<FromElement, ToElement>;


    template<typename Policies, typename FromElement, typename ToElement, Rank rank>
    auto reclassify(
        Policies const& policies,
        PartitionedArray<FromElement, rank> const& array,
        hpx::shared_future<LookupTable<FromElement, ToElement>> const& lookup_table)
        -> PartitionedArray<ToElement, rank>;


    template<typename Policies, typename FromElement, typename ToElement, Rank rank>
    auto reclassify(
        Policies const& policies,
        PartitionedArray<FromElement, rank> const& array,
        LookupTable<FromElement, ToElement> const& lookup_table) -> PartitionedArray<ToElement, rank>
    {
        return reclassify(policies, array, hpx::make_ready_future(lookup_table).share());
    }

}  // namespace lue
