#pragma once
#include "lue/framework/algorithm/focal_high_pass.hpp"


namespace lue {
    namespace policy::focal_high_pass {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_high_pass


    namespace default_policies {

        template<
            typename Element,
            Rank rank,
            typename Kernel>
        PartitionedArray<Element, rank> focal_high_pass(
            PartitionedArray<Element, rank> const& array,
            Kernel const& kernel)
        {
            using Policies = policy::focal_high_pass::DefaultPolicies<Element>;

            Element const fill_value{0};

            return focal_high_pass(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
