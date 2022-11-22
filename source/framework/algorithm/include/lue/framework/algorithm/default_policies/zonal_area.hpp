#pragma once
#include "lue/framework/algorithm/default_policies/zonal_sum.hpp"
#include "lue/framework/algorithm/zonal_area.hpp"


namespace lue {
    namespace policy::zonal_area {

        template<
            typename Count,
            typename Zone>
        using DefaultPolicies = zonal_sum::DefaultPolicies<Count, Zone>;

    }  // namespace policy::zonal_area


    namespace default_policies {

        template<
            typename Count,
            typename Zone,
            Rank rank>
        PartitionedArray<Count, rank> zonal_area(
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_area::DefaultPolicies<Count, Zone>;

            return zonal_area<Count>(Policies{}, zones);
        }

    }  // namespace default_policies
}  // namespace lue
