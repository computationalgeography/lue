#pragma once
#include "lue/framework/algorithm/log.hpp"


namespace lue {
    namespace policy::log {

        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log


    namespace value_policies {

        template<typename Element, Rank rank>
        auto log(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::log::DefaultValuePolicies<Element>;

            return log(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
