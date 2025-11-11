#pragma once
#include "lue/framework/algorithm/focal_mean.hpp"


namespace lue {
    namespace policy::focal_mean {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_mean


    namespace value_policies {

        template<std::floating_point Element, typename Kernel>
            requires std::integral<ElementT<Kernel>> && (rank<Kernel> == 2)
        auto focal_mean(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::focal_mean::DefaultValuePolicies<Element>;

            // TODO This one should be policy-based
            Element const fill_value{policy::no_data_value<Element>};

            return focal_mean(Policies{fill_value}, array, kernel);
        }

    }  // namespace value_policies
}  // namespace lue
