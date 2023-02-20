#pragma once
#include "lue/framework/algorithm/slope.hpp"


namespace lue {
    namespace policy::slope {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::slope


    namespace value_policies {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> slope(
            PartitionedArray<Element, rank> const& elevation, Element const cell_size)
        {
            using Policies = policy::slope::DefaultValuePolicies<Element>;

            return slope(Policies{}, elevation, cell_size);
        }

    }  // namespace value_policies
}  // namespace lue
