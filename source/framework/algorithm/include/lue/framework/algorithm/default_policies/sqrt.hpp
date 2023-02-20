#pragma once
#include "lue/framework/algorithm/sqrt.hpp"


namespace lue {
    namespace policy::sqrt {

        template<typename Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sqrt


    namespace default_policies {

        template<typename Element, Rank rank>
        auto sqrt(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::sqrt::DefaultPolicies<Element>;

            return sqrt(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
