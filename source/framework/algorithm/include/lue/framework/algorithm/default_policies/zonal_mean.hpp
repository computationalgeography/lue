#pragma once
#include "lue/framework/algorithm/zonal_mean.hpp"


namespace lue {
    namespace policy::zonal_mean {

        template<typename Element, typename Zone>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Element>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_mean


    namespace default_policies {

        template<typename Element, typename Zone, Rank rank>
        PartitionedArray<Element, rank> zonal_mean(
            PartitionedArray<Element, rank> const& array, PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_mean::DefaultPolicies<Element, Zone>;

            return zonal_mean(Policies{}, array, zones);
        }

    }  // namespace default_policies
}  // namespace lue
