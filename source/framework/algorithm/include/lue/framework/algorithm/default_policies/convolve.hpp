#pragma once
#include "lue/framework/algorithm/convolve.hpp"


namespace lue {
    namespace policy::convolve {

        template<std::floating_point Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::convolve


    namespace default_policies {

        template<std::floating_point Element, typename Kernel>
        auto convolve(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::convolve::DefaultPolicies<Element>;

            Element const fill_value{0};

            return convolve(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
