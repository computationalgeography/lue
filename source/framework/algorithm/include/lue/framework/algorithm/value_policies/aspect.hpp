#pragma once
#include "lue/framework/algorithm/aspect.hpp"


namespace lue {
    namespace policy::aspect {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace aspect::policy


    namespace value_policies {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> aspect(
            PartitionedArray<Element, rank> const& elevation)
        {
            using Policies = policy::aspect::DefaultValuePolicies<Element>;

            return aspect(Policies{}, elevation);
        }

    }  // namespace value_policies
}  // namespace lue
