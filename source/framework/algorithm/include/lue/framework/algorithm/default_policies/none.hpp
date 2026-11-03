#pragma once
#include "lue/framework/algorithm/none.hpp"


namespace lue {
    namespace policy::none {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::none


    namespace default_policies {

        template<typename Element, Rank rank>
        auto none(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Policies = policy::none::DefaultPolicies<Element>;

            return none(Policies{}, array);
        }


        template<typename Element, Rank rank>
        auto none(hpx::id_type const locality_id, ArrayPartition<Element, rank> const& partition)
            -> Scalar<Element>
        {
            using Policies = policy::none::DefaultPolicies<Element>;

            return none(locality_id, Policies{}, partition);
        }

    }  // namespace default_policies
}  // namespace lue
