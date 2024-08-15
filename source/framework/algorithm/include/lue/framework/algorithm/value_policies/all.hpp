#pragma once
#include "lue/framework/algorithm/all.hpp"


namespace lue {
    namespace policy::all {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::all


    namespace value_policies {

        template<typename Element, Rank rank>
        auto all(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Policies = policy::all::DefaultValuePolicies<Element>;

            return all(Policies{}, array);
        }


        template<typename Element, Rank rank>
        auto all(hpx::id_type const locality_id, ArrayPartition<Element, rank> const& partition)
            -> Scalar<Element>
        {
            using Policies = policy::all::DefaultValuePolicies<Element>;

            return all(locality_id, Policies{}, partition);
        }

    }  // namespace value_policies
}  // namespace lue
