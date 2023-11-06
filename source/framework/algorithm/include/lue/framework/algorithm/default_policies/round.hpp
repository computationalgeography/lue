#pragma once
#include "lue/framework/algorithm/round.hpp"


namespace lue {
    namespace policy::round {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::round


    namespace default_policies {

        template<typename Element, Rank rank>
        auto round(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::round::DefaultPolicies<Element>;

            return round(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
