#pragma once
#include "lue/framework/algorithm/integrate_and_allocate.hpp"


namespace lue {
    namespace policy::integrate_and_allocate {

        template<typename ZoneElement, typename FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<ZoneElement, ZoneElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<ZoneElement, ZoneElement, FloatingPointElement>>;

    }  // namespace policy::integrate_and_allocate


    namespace value_policies {

        template<typename ZoneElement, typename FloatingPointElement, Rank rank>
        auto integrate_and_allocate(
            SerialRoute<ZoneElement, rank> const& route,
            PartitionedArray<ZoneElement, rank> const& zone,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                crop_fractions)
            -> std::tuple<
                std::vector<PartitionedArray<FloatingPointElement, rank>>,
                std::map<ZoneElement, FloatingPointElement>>
        {
            using Policies =
                policy::integrate_and_allocate::DefaultValuePolicies<ZoneElement, FloatingPointElement>;

            return integrate_and_allocate(Policies{}, route, zone, crop_fractions);
        }

    }  // namespace value_policies
}  // namespace lue
