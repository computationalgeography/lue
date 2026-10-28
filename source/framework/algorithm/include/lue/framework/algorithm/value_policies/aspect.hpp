#pragma once
#include "lue/framework/algorithm/aspect.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::aspect {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::aspect


    namespace value_policies {

        template<std::floating_point Element>
        auto aspect(PartitionedArray<Element, 2> const& elevation) -> PartitionedArray<Element, 2>
        {
            using Policies = policy::aspect::DefaultValuePolicies<Element>;

            // TODO: This one should be policy-based
            Element const fill_value{policy::no_data_value<Element>};

            return aspect(Policies{fill_value}, elevation);
        }

    }  // namespace value_policies
}  // namespace lue
