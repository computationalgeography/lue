#pragma once
#include "lue/framework/algorithm/default_policies/zonal_sum.hpp"
#include "lue/framework/algorithm/zonal_normal.hpp"


namespace lue {
    namespace policy::zonal_normal {

        template<typename Element, typename Zone>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Zone>, OutputElements<Element>, InputElements<Zone>>;

    }  // namespace policy::zonal_normal


    namespace default_policies {

        template<typename Element, typename Zone, Rank rank>
        PartitionedArray<Element, rank> zonal_normal(PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_normal::DefaultPolicies<Element, Zone>;

            return zonal_normal<Element>(Policies{}, zones);
        }

    }  // namespace default_policies
}  // namespace lue
