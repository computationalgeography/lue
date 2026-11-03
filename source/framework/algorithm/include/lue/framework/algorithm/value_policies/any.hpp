#pragma once
#include "lue/framework/algorithm/any.hpp"


namespace lue {
    namespace policy::any {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::any


    namespace value_policies {

        template<typename Element, Rank rank>
        auto any(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Policies = policy::any::DefaultValuePolicies<Element>;

            return any(Policies{}, array);
        }


        template<typename Element, Rank rank>
        auto any(hpx::id_type const locality_id, ArrayPartition<Element, rank> const& partition)
            -> Scalar<Element>
        {
            using Policies = policy::any::DefaultValuePolicies<Element>;

            return any(locality_id, Policies{}, partition);
        }

    }  // namespace value_policies
}  // namespace lue
