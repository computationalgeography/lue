#pragma once
#include "lue/framework/algorithm/focal_maximum.hpp"
#include <limits>


namespace lue {
    namespace policy::focal_maximum {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_maximum


    namespace default_policies {

        template<
            typename Element,
            Rank rank,
            typename Kernel>
        PartitionedArray<Element, rank> focal_maximum(
            PartitionedArray<Element, rank> const& array,
            Kernel const& kernel)
        {
            using Policies = policy::focal_maximum::DefaultPolicies<Element>;

            Element const fill_value{std::numeric_limits<Element>::min()};

            return focal_maximum(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
