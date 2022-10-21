#pragma once
#include "lue/framework/algorithm/focal_mean.hpp"


namespace lue {
    namespace policy::focal_mean {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace focal_mean::policy


    namespace value_policies {

        template<
            typename Element,
            Rank rank,
            typename Kernel>
        PartitionedArray<Element, rank> focal_mean(
            PartitionedArray<Element, rank> const& array,
            Kernel const& kernel)
        {
            using Policies = policy::focal_mean::DefaultValuePolicies<Element>;

            return focal_mean(Policies{}, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
