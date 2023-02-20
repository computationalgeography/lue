#pragma once
#include "lue/framework/algorithm/asin.hpp"


namespace lue {
    namespace policy::asin {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::asin


    namespace default_policies {

        template<typename Element, Rank rank>
        auto asin(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::asin::DefaultPolicies<Element>;

            return asin(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
