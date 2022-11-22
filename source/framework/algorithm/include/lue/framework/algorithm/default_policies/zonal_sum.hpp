#pragma once
#include "lue/framework/algorithm/zonal_sum.hpp"


namespace lue {
    namespace policy::zonal_sum {

        template<
            typename Element,
            typename Zone>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Zone>,
            OutputElements<Element>,
            InputElements<Element, Zone>>;

    }  // namespace policy::zonal_sum


    namespace default_policies {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_sum(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_sum::DefaultPolicies<Element, Zone>;

            return zonal_sum(Policies{}, array, zones);
        }

    }  // namespace default_policies
}  // namespace lue
