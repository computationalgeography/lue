#pragma once
#include "lue/framework/algorithm/acos.hpp"


namespace lue {
    namespace policy::acos {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::acos


    namespace default_policies {

        template<
            typename Element,
            Rank rank>
        auto acos(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::acos::DefaultPolicies<Element>;

            return acos(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
