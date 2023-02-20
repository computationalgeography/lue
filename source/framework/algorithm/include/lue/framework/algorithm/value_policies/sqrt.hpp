#pragma once
#include "lue/framework/algorithm/sqrt.hpp"


namespace lue {
    namespace policy::sqrt {

        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sqrt


    namespace value_policies {

        template<typename Element, Rank rank>
        auto sqrt(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::sqrt::DefaultValuePolicies<Element>;

            return sqrt(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
