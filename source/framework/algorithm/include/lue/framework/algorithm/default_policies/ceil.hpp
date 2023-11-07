#pragma once
#include "lue/framework/algorithm/ceil.hpp"


namespace lue {
    namespace policy::ceil {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::ceil


    namespace default_policies {

        template<typename Element, Rank rank>
        auto ceil(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::ceil::DefaultPolicies<Element>;

            return ceil(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
