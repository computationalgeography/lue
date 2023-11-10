#pragma once
#include "lue/framework/algorithm/round.hpp"


namespace lue {
    namespace policy::round {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::round


    namespace value_policies {

        template<typename Element, Rank rank>
        auto round(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::round::DefaultValuePolicies<Element>;

            return round(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
