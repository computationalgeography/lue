#pragma once
#include "lue/framework/algorithm/focal_mean.hpp"


namespace lue {
    namespace policy::focal_mean {

        template<typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_mean


    namespace default_policies {

        template<typename Element, Rank rank, typename Kernel>
        PartitionedArray<Element, rank> focal_mean(
            PartitionedArray<Element, rank> const& array, Kernel const& kernel)
        {
            using Policies = policy::focal_mean::DefaultPolicies<Element>;

            Element const fill_value{0};

            return focal_mean(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
