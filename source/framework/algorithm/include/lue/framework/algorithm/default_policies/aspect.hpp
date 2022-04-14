#pragma once
#include "lue/framework/algorithm/aspect.hpp"


namespace lue {
    namespace policy::aspect {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace aspect::policy


    namespace default_policies {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> aspect(
            PartitionedArray<Element, rank> const& elevation)
        {
            using Policies = policy::aspect::DefaultPolicies<Element>;

            return aspect(Policies{}, elevation);
        }

    }  // namespace default_policies
}  // namespace lue
