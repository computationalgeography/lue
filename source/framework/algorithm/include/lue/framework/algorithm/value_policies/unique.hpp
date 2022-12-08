#pragma once
#include "lue/framework/algorithm/unique.hpp"


namespace lue {
    namespace policy::unique {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::unique


    namespace value_policies {

        template<
            typename Element,
            Rank rank>
        hpx::future<std::set<Element>> unique(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::unique::DefaultValuePolicies<Element>;

            return lue::unique(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
