#pragma once
#include "lue/framework/algorithm/sum.hpp"


namespace lue {
    namespace policy::sum {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::sum


    namespace value_policies {

        template<typename Element, Rank rank>
        auto sum(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Policies = policy::sum::DefaultValuePolicies<Element>;

            return sum(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
