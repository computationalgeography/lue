#pragma once
#include "lue/framework/algorithm/convolve.hpp"


namespace lue {
    namespace policy::convolve {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::convolve


    namespace value_policies {

        template<std::floating_point Element, typename Kernel>
        auto convolve(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::convolve::DefaultValuePolicies<Element>;

            // TODO: This one should be policy-based
            Element const fill_value{policy::no_data_value<Element>};

            return convolve(Policies{fill_value}, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
