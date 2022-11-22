#pragma once
#include "lue/framework/algorithm/zonal_minimum.hpp"


namespace lue {
    namespace policy::zonal_minimum {

        template<
            typename Element,
            typename Zone>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Element>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_minimum


    namespace value_policies {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_minimum(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_minimum::DefaultValuePolicies<Element, Zone>;

            return zonal_minimum(Policies{}, array, zones);
        }

    }  // namespace value_policies
}  // namespace lue
