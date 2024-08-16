#pragma once
#include "lue/framework/algorithm/unique.hpp"


namespace lue {
    namespace policy::unique {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::unique


    namespace value_policies {

        template<typename Element, Rank rank>
        auto unique(PartitionedArray<Element, rank> const& array) -> hpx::future<std::set<Element>>
        {
            using Policies = policy::unique::DefaultValuePolicies<Element>;

            return lue::unique(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
