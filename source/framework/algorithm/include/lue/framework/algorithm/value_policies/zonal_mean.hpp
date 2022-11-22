#pragma once
#include "lue/framework/algorithm/zonal_mean.hpp"


namespace lue {
    namespace policy::zonal_mean {

        template<
            typename Element,
            typename Zone>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Element>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_mean


    namespace value_policies {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_mean(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_mean::DefaultValuePolicies<Element, Zone>;

            return zonal_mean(Policies{}, array, zones);
        }

    }  // namespace value_policies
}  // namespace lue
