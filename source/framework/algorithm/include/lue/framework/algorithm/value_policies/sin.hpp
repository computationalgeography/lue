#pragma once
#include "lue/framework/algorithm/sin.hpp"


namespace lue {
    namespace policy::sin {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::sin


    namespace value_policies {

        template<
            typename Element,
            Rank rank>
        auto sin(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::sin::DefaultValuePolicies<Element>;

            return sin(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
