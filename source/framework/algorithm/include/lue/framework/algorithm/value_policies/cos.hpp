#pragma once
#include "lue/framework/algorithm/cos.hpp"


namespace lue {
    namespace policy::cos {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::cos


    namespace value_policies {

        template<typename Element, Rank rank>
        auto cos(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::cos::DefaultValuePolicies<Element>;

            return cos(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
