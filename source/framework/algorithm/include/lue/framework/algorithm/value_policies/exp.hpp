#pragma once
#include "lue/framework/algorithm/exp.hpp"


namespace lue {
    namespace policy::exp {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::exp


    namespace value_policies {

        template<
            typename Element,
            Rank rank>
        auto exp(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::exp::DefaultValuePolicies<Element>;

            return exp(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
