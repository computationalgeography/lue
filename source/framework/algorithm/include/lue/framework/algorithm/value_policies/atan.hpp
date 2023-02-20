#pragma once
#include "lue/framework/algorithm/atan.hpp"


namespace lue {
    namespace policy::atan {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::atan


    namespace value_policies {

        template<typename Element, Rank rank>
        auto atan(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::atan::DefaultValuePolicies<Element>;

            return atan(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
