#pragma once
#include "lue/framework/algorithm/exp.hpp"


namespace lue {
    namespace policy::exp {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::exp


    namespace default_policies {

        template<
            typename Element,
            Rank rank>
        auto exp(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::exp::DefaultPolicies<Element>;

            return exp(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
