#pragma once
#include "lue/framework/algorithm/maximum.hpp"


namespace lue {
    namespace policy::maximum {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::maximum


    namespace default_policies {

        template<
            typename Element,
            Rank rank>
        hpx::future<Element> maximum(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::maximum::DefaultPolicies<Element>;

            return maximum(Policies{}, array);
        }

    }  // namespace default_policies

}  // namespace lue
