#pragma once
#include "lue/framework/algorithm/clump.hpp"


namespace lue {
    namespace policy::clump {

        template<typename ZoneElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<ZoneElement>,
            OutputElements<ZoneElement>,
            InputElements<ZoneElement>>;

    }  // namespace policy::clump


    namespace default_policies {

        template<typename ZoneElement, Rank rank>
        auto clump(PartitionedArray<ZoneElement, rank> const& zone) -> PartitionedArray<ZoneElement, rank>
        {
            using Policies = policy::clump::DefaultPolicies<ZoneElement>;

            return clump(Policies{}, zone);
        }

    }  // namespace default_policies
}  // namespace lue
