#pragma once
#include "lue/framework/algorithm/focal_sum.hpp"


namespace lue {
    namespace policy::focal_sum {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace focal_sum::policy


    namespace value_policies {

        template<
            typename Element,
            Rank rank,
            typename Kernel>
        PartitionedArray<Element, rank> focal_sum(
            PartitionedArray<Element, rank> const& array,
            Kernel const& kernel)
        {
            using Policies = policy::focal_sum::DefaultValuePolicies<Element>;

            return focal_sum(Policies{}, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
