#pragma once
#include "lue/framework/algorithm/slope.hpp"


namespace lue {
    namespace policy::slope {

        template<typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::slope


    namespace default_policies {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> slope(
            PartitionedArray<Element, rank> const& elevation, Element const cell_size)
        {
            using Policies = policy::slope::DefaultPolicies<Element>;

            return slope(Policies{}, elevation, cell_size);
        }

    }  // namespace default_policies
}  // namespace lue
