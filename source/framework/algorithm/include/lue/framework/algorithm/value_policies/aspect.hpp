#pragma once
#include "lue/framework/algorithm/aspect.hpp"


namespace lue {
    namespace policy::aspect {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::aspect


    namespace value_policies {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> aspect(PartitionedArray<Element, rank> const& elevation)
        {
            using Policies = policy::aspect::DefaultValuePolicies<Element>;

            // TODO This one should be policy-based
            Element const fill_value{policy::no_data_value<Element>};

            return aspect(Policies{fill_value}, elevation);
        }

    }  // namespace value_policies
}  // namespace lue
