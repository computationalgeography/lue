#pragma once
#include "lue/framework/algorithm/zonal_maximum.hpp"


namespace lue {
    namespace policy::zonal_maximum {

        template<
            typename Element,
            typename Zone>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Element>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_maximum


    namespace value_policies {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_maximum(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_maximum::DefaultValuePolicies<Element, Zone>;

            return zonal_maximum(Policies{}, array, zones);
        }

    }  // namespace value_policies
}  // namespace lue
