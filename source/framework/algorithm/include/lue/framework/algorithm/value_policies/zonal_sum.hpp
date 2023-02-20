#pragma once
#include "lue/framework/algorithm/zonal_sum.hpp"


namespace lue {
    namespace policy::zonal_sum {

        template<typename Element, typename Zone>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Element>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_sum


    namespace value_policies {

        template<typename Element, typename Zone, Rank rank>
        PartitionedArray<Element, rank> zonal_sum(
            PartitionedArray<Element, rank> const& array, PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_sum::DefaultValuePolicies<Element, Zone>;

            return zonal_sum(Policies{}, array, zones);
        }

    }  // namespace value_policies
}  // namespace lue
