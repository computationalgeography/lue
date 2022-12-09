#pragma once
#include "lue/framework/algorithm/zonal_uniform.hpp"


namespace lue {
    namespace policy::zonal_uniform {

        template<
            typename Element,
            typename Zone>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Zone>,
            OutputElements<Element>,
            InputElements<Zone>>;

    }  // namespace policy::zonal_uniform


    namespace default_policies {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_uniform(
            PartitionedArray<Zone, rank> const& zones)
        {
            using Policies = policy::zonal_uniform::DefaultPolicies<Element, Zone>;

            return zonal_uniform<Element>(Policies{}, zones);
        }

    }  // namespace default_policies
}  // namespace lue
