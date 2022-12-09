#pragma once
#include "lue/framework/algorithm/zonal_normal.hpp"


namespace lue {
    namespace policy::zonal_normal {

        template<
            typename Element,
            typename Zone>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Zone>,
            OutputElements<Element>,
            InputElements<Zone>>;

    }  // namespace policy::zonal_normal


    namespace value_policies {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_normal(
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_normal::DefaultValuePolicies<Element, Zone>;

            return zonal_normal<Element>(Policies{}, zones);
        }

    }  // namespace value_policies
}  // namespace lue
