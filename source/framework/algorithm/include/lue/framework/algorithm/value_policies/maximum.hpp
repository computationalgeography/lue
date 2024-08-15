#pragma once
#include "lue/framework/algorithm/maximum.hpp"


namespace lue {
    namespace policy::maximum {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::maximum


    namespace value_policies {

        template<typename Element, Rank rank>
        auto maximum(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Policies = policy::maximum::DefaultValuePolicies<Element>;

            return maximum(Policies{}, array);
        }

    }  // namespace value_policies

}  // namespace lue
