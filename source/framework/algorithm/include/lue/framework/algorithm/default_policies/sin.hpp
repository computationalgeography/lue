#pragma once
#include "lue/framework/algorithm/sin.hpp"


namespace lue {
    namespace policy::sin {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sin


    namespace default_policies {

        template<typename Element, Rank rank>
        auto sin(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::sin::DefaultPolicies<Element>;

            return sin(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
