#pragma once
#include "lue/framework/algorithm/zonal_uniform.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::zonal_uniform {

        template<typename Element, typename Zone>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<Zone>, OutputElements<Element>, InputElements<Zone>>;

    }  // namespace policy::zonal_uniform


    namespace value_policies {

        template<typename Element, typename Zone, Rank rank>
        auto zonal_uniform(PartitionedArray<Zone, rank> const& zones) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::zonal_uniform::DefaultValuePolicies<Element, Zone>;

            return zonal_uniform<Element>(Policies{}, zones);
        }

    }  // namespace value_policies
}  // namespace lue
