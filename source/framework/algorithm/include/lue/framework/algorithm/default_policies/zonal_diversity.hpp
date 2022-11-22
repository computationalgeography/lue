#pragma once
#include "lue/framework/algorithm/zonal_diversity.hpp"


namespace lue {
    namespace policy::zonal_diversity {

        template<
            typename Count,
            typename Element,
            typename Zone>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Count>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_diversity


    namespace default_policies {

        template<
            typename Count,
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Count, rank> zonal_diversity(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_diversity::DefaultPolicies<Count, Element, Zone>;

            return zonal_diversity<Count>(Policies{}, array, zones);
        }

    }  // namespace default_policies
}  // namespace lue
