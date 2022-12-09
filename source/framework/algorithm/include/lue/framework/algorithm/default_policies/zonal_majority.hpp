#pragma once
#include "lue/framework/algorithm/zonal_majority.hpp"


namespace lue {
    namespace policy::zonal_majority {

        template<
            typename Element,
            typename Zone>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Element>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_majority


    namespace default_policies {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_majority(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_majority::DefaultPolicies<Element, Zone>;

            return zonal_majority(Policies{}, array, zones);
        }

    }  // namespace default_policies
}  // namespace lue
