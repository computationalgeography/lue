#pragma once
#include "lue/framework/algorithm/log.hpp"


namespace lue {
    namespace policy::log {

        template<typename Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log


    namespace default_policies {

        template<typename Element, Rank rank>
        auto log(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::log::DefaultPolicies<Element>;

            return log(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
