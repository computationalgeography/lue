#pragma once
#include "lue/framework/algorithm/focal_majority.hpp"


namespace lue {
    namespace policy::focal_majority {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_majority


    namespace default_policies {

        template<
            typename Element,
            Rank rank,
            typename Kernel>
        PartitionedArray<Element, rank> focal_majority(
            PartitionedArray<Element, rank> const& array,
            Kernel const& kernel)
        {
            using Policies = policy::focal_majority::DefaultPolicies<Element>;

            Element const fill_value{0};

            return focal_majority(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
