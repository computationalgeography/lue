#pragma once
#include "lue/framework/algorithm/floor.hpp"


namespace lue {
    namespace policy::floor {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::floor


    namespace value_policies {

        template<typename Element, Rank rank>
        auto floor(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::floor::DefaultValuePolicies<Element>;

            return floor(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
