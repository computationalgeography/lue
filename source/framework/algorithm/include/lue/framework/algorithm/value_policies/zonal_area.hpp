#pragma once
#include "lue/framework/algorithm/value_policies/zonal_sum.hpp"
#include "lue/framework/algorithm/zonal_area.hpp"


namespace lue {
    namespace policy::zonal_area {

        template<typename Count, typename Zone>
        using DefaultValuePolicies = zonal_sum::DefaultValuePolicies<Count, Zone>;

    }  // namespace policy::zonal_area


    namespace value_policies {

        template<typename Count, typename Zone, Rank rank>
        PartitionedArray<Count, rank> zonal_area(PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_area::DefaultValuePolicies<Count, Zone>;

            return zonal_area<Count>(Policies{}, zones);
        }

    }  // namespace value_policies
}  // namespace lue
