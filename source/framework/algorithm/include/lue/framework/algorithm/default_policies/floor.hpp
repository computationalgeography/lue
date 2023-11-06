#pragma once
#include "lue/framework/algorithm/floor.hpp"


namespace lue {
    namespace policy::floor {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::floor


    namespace default_policies {

        template<typename Element, Rank rank>
        auto floor(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::floor::DefaultPolicies<Element>;

            return floor(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
