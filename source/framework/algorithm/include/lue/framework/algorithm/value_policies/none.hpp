#pragma once
#include "lue/framework/algorithm/none.hpp"


namespace lue {
    namespace policy::none {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::none


    namespace value_policies {

        template<typename Element, Rank rank>
        auto none(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Policies = policy::none::DefaultValuePolicies<Element>;

            return none(Policies{}, array);
        }


        template<typename Element, Rank rank>
        auto none(hpx::id_type const locality_id, ArrayPartition<Element, rank> const& partition)
            -> Scalar<Element>
        {
            using Policies = policy::none::DefaultValuePolicies<Element>;

            return none(locality_id, Policies{}, partition);
        }

    }  // namespace value_policies
}  // namespace lue
