#pragma once
#include "lue/framework/algorithm/sum.hpp"


namespace lue {
    namespace policy::sum {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sum


    namespace default_policies {

        template<typename Element, Rank rank>
        auto sum(PartitionedArray<Element, rank> const& array) -> hpx::future<Element>
        {
            using Policies = policy::sum::DefaultPolicies<Element>;

            return sum(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
