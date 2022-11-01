#pragma once
#include "lue/framework/algorithm/focal_sum.hpp"


namespace lue {
    namespace policy::focal_sum {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_sum


    namespace default_policies {

        template<
            typename Element,
            Rank rank,
            typename Kernel>
        PartitionedArray<Element, rank> focal_sum(
            PartitionedArray<Element, rank> const& array,
            Kernel const& kernel)
        {
            using Policies = policy::focal_sum::DefaultPolicies<Element>;

            return focal_sum(Policies{}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
