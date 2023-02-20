#pragma once
#include "lue/framework/algorithm/tan.hpp"


namespace lue {
    namespace policy::tan {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::tan


    namespace default_policies {

        template<typename Element, Rank rank>
        auto tan(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::tan::DefaultPolicies<Element>;

            return tan(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
