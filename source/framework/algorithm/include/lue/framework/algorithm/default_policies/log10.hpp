#pragma once
#include "lue/framework/algorithm/log10.hpp"


namespace lue {
    namespace policy::log10 {

        template<typename Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log10


    namespace default_policies {

        template<typename Element, Rank rank>
        auto log10(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::log10::DefaultPolicies<Element>;

            return log10(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
