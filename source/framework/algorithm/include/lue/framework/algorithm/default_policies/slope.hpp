#pragma once
#include "lue/framework/algorithm/slope.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::slope {

        template<std::floating_point Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::slope


    namespace default_policies {

        template<std::floating_point Element>
        auto slope(PartitionedArray<Element, 2> const& elevation, Element const cell_size)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::slope::DefaultPolicies<Element>;

            return slope(Policies{}, elevation, cell_size);
        }

    }  // namespace default_policies
}  // namespace lue
