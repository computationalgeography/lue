#pragma once
#include "lue/framework/algorithm/focal_max.hpp"
#include <limits>


namespace lue {
    namespace policy::focal_max {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_max


    namespace value_policies {

        template<
            typename Element,
            Rank rank,
            typename Kernel>
        PartitionedArray<Element, rank> focal_max(
            PartitionedArray<Element, rank> const& array,
            Kernel const& kernel)
        {
            using Policies = policy::focal_max::DefaultValuePolicies<Element>;

            Element const fill_value{std::numeric_limits<Element>::min()};

            return focal_max(Policies{fill_value}, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
