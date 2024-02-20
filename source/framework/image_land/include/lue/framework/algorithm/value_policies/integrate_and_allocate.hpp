#pragma once
#include "lue/framework/algorithm/integrate_and_allocate.hpp"


namespace lue {
    namespace policy::integrate_and_allocate {

        template<typename ZoneElement, typename FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<
                ZoneElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<
                ZoneElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement>>;

    }  // namespace policy::integrate_and_allocate


    namespace value_policies {

        template<typename ZoneElement, typename FloatingPointElement, Rank rank>
        auto integrate_and_allocate(
            SerialRoute<ZoneElement, rank> const& routes,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                sdp_factors,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                yield_factors,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                crop_fractions,
            std::map<ZoneElement, std::vector<FloatingPointElement>> const& demands,
            std::map<ZoneElement, std::vector<FloatingPointElement>> const& current_production,
            PartitionedArray<FloatingPointElement, rank> const& irrigated_crop_fractions)
            -> std::tuple<
                std::vector<PartitionedArray<FloatingPointElement, rank>>,
                std::map<ZoneElement, std::vector<FloatingPointElement>>>
        {
            using Policies =
                policy::integrate_and_allocate::DefaultValuePolicies<ZoneElement, FloatingPointElement>;

            return integrate_and_allocate(
                Policies{},
                routes,
                sdp_factors,
                yield_factors,
                crop_fractions,
                demands,
                current_production,
                irrigated_crop_fractions);
        }

    }  // namespace value_policies
}  // namespace lue
