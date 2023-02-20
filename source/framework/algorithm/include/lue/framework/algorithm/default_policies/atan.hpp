#pragma once
#include "lue/framework/algorithm/atan.hpp"


namespace lue {
    namespace policy::atan {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::atan


    namespace default_policies {

        template<typename Element, Rank rank>
        auto atan(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::atan::DefaultPolicies<Element>;

            return atan(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
