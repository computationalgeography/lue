#pragma once
#include "lue/framework/algorithm/ceil.hpp"


namespace lue {
    namespace policy::ceil {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::ceil


    namespace value_policies {

        template<typename Element, Rank rank>
        auto ceil(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::ceil::DefaultValuePolicies<Element>;

            return ceil(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
