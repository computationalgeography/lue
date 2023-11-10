#pragma once
#include "lue/framework/algorithm/log10.hpp"


namespace lue {
    namespace policy::log10 {

        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log10


    namespace value_policies {

        template<typename Element, Rank rank>
        auto log10(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::log10::DefaultValuePolicies<Element>;

            return log10(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
