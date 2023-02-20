#pragma once
#include "lue/framework/algorithm/cos.hpp"


namespace lue {
    namespace policy::cos {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::cos


    namespace default_policies {

        template<typename Element, Rank rank>
        auto cos(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::cos::DefaultPolicies<Element>;

            return cos(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
