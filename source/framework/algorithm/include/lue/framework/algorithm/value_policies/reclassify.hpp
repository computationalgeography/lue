#pragma once
#include "lue/framework/algorithm/reclassify.hpp"


namespace lue {
    namespace policy::reclassify {

        template<typename FromElement, typename ToElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FromElement>,
            OutputElements<ToElement>,
            InputElements<FromElement>>;

    }  // namespace policy::reclassify


    namespace value_policies {

        template<typename FromElement, typename ToElement, Rank rank>
        PartitionedArray<ToElement, rank> reclassify(
            PartitionedArray<FromElement, rank> const& array,
            hpx::shared_future<LookupTable<FromElement, ToElement>> const& lookup_table)
        {
            using Policies = policy::reclassify::DefaultValuePolicies<FromElement, ToElement>;

            return reclassify(Policies{}, array, lookup_table);
        }

    }  // namespace value_policies
}  // namespace lue
