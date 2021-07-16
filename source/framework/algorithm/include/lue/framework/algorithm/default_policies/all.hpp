#pragma once
#include "lue/framework/algorithm/all.hpp"


namespace lue {
    namespace policy::all {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::all


    namespace default_policies {

        template<
            typename Element,
            Rank rank>
        hpx::future<Element> all(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::all::DefaultPolicies<Element>;

            return all(Policies{}, array);
        }


        template<
            typename Element,
            Rank rank>
        hpx::future<Element> all(
            hpx::id_type const locality_id,
            ArrayPartition<Element, rank> const& partition)
        {
            using Policies = policy::all::DefaultPolicies<Element>;

            return all(locality_id, Policies{}, partition);
        }

    }  // namespace default_policies
}  // namespace lue
