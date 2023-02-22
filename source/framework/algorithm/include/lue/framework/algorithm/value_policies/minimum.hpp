#pragma once
#include "lue/framework/algorithm/minimum.hpp"


namespace lue {
    namespace policy::minimum {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::minimum


    namespace value_policies {

        template<typename Element, Rank rank>
        hpx::future<Element> minimum(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::minimum::DefaultValuePolicies<Element>;

            return minimum(Policies{}, array);
        }

    }  // namespace value_policies

}  // namespace lue
