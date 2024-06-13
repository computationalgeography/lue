#pragma once
#include "lue/framework/algorithm/maximum.hpp"


namespace lue {
    namespace policy::maximum {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::maximum


    namespace default_policies {

        template<typename Element, Rank rank>
        auto maximum(PartitionedArray<Element, rank> const& array) -> hpx::future<Element>
        {
            using Policies = policy::maximum::DefaultPolicies<Element>;

            return maximum(Policies{}, array);
        }

    }  // namespace default_policies

}  // namespace lue
