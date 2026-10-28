#pragma once
#include "lue/framework/algorithm/aspect.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::aspect {

        template<std::floating_point Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::aspect


    namespace default_policies {

        template<std::floating_point Element>
        auto aspect(PartitionedArray<Element, 2> const& elevation) -> PartitionedArray<Element, 2>
        {
            using Policies = policy::aspect::DefaultPolicies<Element>;

            return aspect(Policies{}, elevation);
        }

    }  // namespace default_policies
}  // namespace lue
