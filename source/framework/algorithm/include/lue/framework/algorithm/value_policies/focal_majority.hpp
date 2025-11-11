#pragma once
#include "lue/framework/algorithm/focal_majority.hpp"


namespace lue {
    namespace policy::focal_majority {

        template<std::integral Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_majority


    namespace value_policies {

        template<std::integral Element, typename Kernel>
        auto focal_majority(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::focal_majority::DefaultValuePolicies<Element>;

            // TODO This one should be policy-based
            Element const fill_value{policy::no_data_value<Element>};

            return focal_majority(Policies{fill_value}, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
