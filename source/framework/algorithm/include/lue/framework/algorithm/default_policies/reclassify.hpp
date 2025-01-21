#pragma once
#include "lue/framework/algorithm/reclassify.hpp"


namespace lue {
    namespace policy::reclassify {

        template<typename FromElement, typename ToElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FromElement>,
            OutputElements<ToElement>,
            InputElements<FromElement>>;

    }  // namespace policy::reclassify


    namespace default_policies {

        template<typename FromElement, typename ToElement, Rank rank>
        auto reclassify(
            PartitionedArray<FromElement, rank> const& array,
            hpx::shared_future<LookupTable<FromElement, ToElement>> const& lookup_table)
            -> PartitionedArray<ToElement, rank>
        {
            using Policies = policy::reclassify::DefaultPolicies<FromElement, ToElement>;

            return reclassify(Policies{}, array, lookup_table);
        }


        template<typename FromElement, typename ToElement, Rank rank>
        auto reclassify(
            PartitionedArray<FromElement, rank> const& array,
            LookupTable<FromElement, ToElement> const& lookup_table) -> PartitionedArray<ToElement, rank>
        {
            using Policies = policy::reclassify::DefaultPolicies<FromElement, ToElement>;

            return reclassify(Policies{}, array, lookup_table);
        }

    }  // namespace default_policies
}  // namespace lue
