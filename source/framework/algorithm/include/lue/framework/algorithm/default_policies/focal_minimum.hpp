#pragma once
#include "lue/framework/algorithm/focal_minimum.hpp"
#include <limits>


namespace lue {
    namespace policy::focal_minimum {

        template<typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_minimum


    namespace default_policies {

        template<typename Element, Rank rank, typename Kernel>
        PartitionedArray<Element, rank> focal_minimum(
            PartitionedArray<Element, rank> const& array, Kernel const& kernel)
        {
            using Policies = policy::focal_minimum::DefaultPolicies<Element>;

            Element const fill_value{std::numeric_limits<Element>::max()};

            return focal_minimum(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
