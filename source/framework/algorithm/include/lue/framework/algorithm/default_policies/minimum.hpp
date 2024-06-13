#pragma once
#include "lue/framework/algorithm/minimum.hpp"


namespace lue {
    namespace policy::minimum {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::minimum


    namespace default_policies {

        template<typename Element, Rank rank>
        auto minimum(PartitionedArray<Element, rank> const& array) -> hpx::future<Element>
        {
            using Policies = policy::minimum::DefaultPolicies<Element>;

            return minimum(Policies{}, array);
        }

    }  // namespace default_policies

}  // namespace lue
