#pragma once
#include "lue/framework/algorithm/gradients.hpp"


namespace lue {
    namespace policy::gradients {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultSpatialOperationValuePolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::gradients


    namespace value_policies {

        template<typename Element, Rank rank>
        Gradients<Element> gradients(
            PartitionedArray<Element, rank> const& elevation, Element const cell_size)
        {
            using Policies = policy::gradients::DefaultValuePolicies<Element>;

            return gradients(Policies{}, elevation, cell_size);
        }

    }  // namespace value_policies
}  // namespace lue
