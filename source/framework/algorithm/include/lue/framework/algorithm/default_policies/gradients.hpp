#pragma once
#include "lue/framework/algorithm/gradients.hpp"


namespace lue {
    namespace policy::gradients {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace gradients::policy


    namespace default_policies {

        template<
            typename Element,
            Rank rank>
        Gradients<Element> gradients(
            PartitionedArray<Element, rank> const& elevation,
            Element const cell_size)
        {
            using Policies = policy::gradients::DefaultPolicies<Element>;

            return gradients(Policies{}, elevation, cell_size);
        }

    }  // namespace default_policies
}  // namespace lue
