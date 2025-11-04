#pragma once
#include "lue/framework/algorithm/focal_maximum.hpp"


namespace lue {
    namespace policy::focal_maximum {

        template<Arithmetic Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_maximum


    namespace value_policies {

        template<Arithmetic Element, typename Kernel>
        auto focal_maximum(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::focal_maximum::DefaultValuePolicies<Element>;

            // TODO This one should be policy-based
            Element const fill_value{policy::no_data_value<Element>};

            return focal_maximum(Policies{fill_value}, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
