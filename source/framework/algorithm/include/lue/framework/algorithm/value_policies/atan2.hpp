#pragma once
#include "lue/framework/algorithm/atan2.hpp"


namespace lue {
    namespace policy::atan2 {

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::atan2


    namespace value_policies {

        template<
            typename Element,
            Rank rank>
        auto atan2(
            PartitionedArray<Element, rank> const& array1,
            PartitionedArray<Element, rank> const& array2)
        {
            using Policies = policy::atan2::DefaultValuePolicies<Element>;

            return atan2(Policies{}, array1, array2);
        }

    }  // namespace value_policies
}  // namespace lue
