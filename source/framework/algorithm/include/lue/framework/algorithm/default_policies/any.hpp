#pragma once
#include "lue/framework/algorithm/any.hpp"


namespace lue {
    namespace policy::any {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::any


    namespace default_policies {

        template<typename Element, Rank rank>
        auto any(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Policies = policy::any::DefaultPolicies<Element>;

            return any(Policies{}, array);
        }


        template<typename Element, Rank rank>
        auto any(hpx::id_type const locality_id, ArrayPartition<Element, rank> const& partition)
            -> Scalar<Element>
        {
            using Policies = policy::any::DefaultPolicies<Element>;

            return any(locality_id, Policies{}, partition);
        }

    }  // namespace default_policies
}  // namespace lue
