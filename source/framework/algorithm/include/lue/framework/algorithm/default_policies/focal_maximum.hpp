#pragma once
#include "lue/framework/algorithm/focal_maximum.hpp"
#include <limits>


namespace lue {
    namespace policy::focal_maximum {

        template<Arithmetic Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_maximum


    namespace default_policies {

        template<Arithmetic Element, typename Kernel>
        auto focal_maximum(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::focal_maximum::DefaultPolicies<Element>;

            Element const fill_value{std::numeric_limits<Element>::min()};

            return focal_maximum(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
