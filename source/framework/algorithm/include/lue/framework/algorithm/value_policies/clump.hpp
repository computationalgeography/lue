#pragma once
#include "lue/framework/algorithm/clump.hpp"


namespace lue {
    namespace policy::clump {

        template<typename ZoneElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<ZoneElement>,
            OutputElements<ZoneElement>,
            InputElements<ZoneElement>>;

    }  // namespace policy::clump


    namespace value_policies {

        template<typename ZoneElement, Rank rank>
        auto clump(PartitionedArray<ZoneElement, rank> const& zone) -> PartitionedArray<ZoneElement, rank>
        {
            using Policies = policy::clump::DefaultValuePolicies<ZoneElement>;

            return clump(Policies{}, zone);
        }

    }  // namespace value_policies
}  // namespace lue
