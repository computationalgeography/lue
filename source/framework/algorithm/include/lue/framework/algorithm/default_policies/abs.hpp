#pragma once
#include "lue/framework/algorithm/abs.hpp"


namespace lue {
    namespace policy::abs {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::abs


    namespace default_policies {

        template<
            typename Element,
            Rank rank>
        auto abs(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::abs::DefaultPolicies<Element>;

            return abs(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
