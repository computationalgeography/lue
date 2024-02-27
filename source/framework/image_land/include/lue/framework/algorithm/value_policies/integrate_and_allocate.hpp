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
                sdp_factors_per_crop,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                yield_factors_per_crop,
            std::vector<std::reference_wrapper<PartitionedArray<FloatingPointElement, rank> const>> const&
                initial_crop_fractions_per_crop,
            hpx::shared_future<std::map<ZoneElement, std::vector<FloatingPointElement>>> const&
                zonal_demands_per_crop,
            hpx::shared_future<std::map<ZoneElement, std::vector<FloatingPointElement>>> const&
                current_zonal_production_per_crop,
            PartitionedArray<FloatingPointElement, rank> const& irrigated_crop_fractions)
            -> std::tuple<
                std::vector<PartitionedArray<FloatingPointElement, rank>>,
                hpx::shared_future<std::map<ZoneElement, std::vector<FloatingPointElement>>>>
        {
            using Policies =
                policy::integrate_and_allocate::DefaultValuePolicies<ZoneElement, FloatingPointElement>;

            return integrate_and_allocate(
                Policies{},
                routes,
                sdp_factors_per_crop,
                yield_factors_per_crop,
                initial_crop_fractions_per_crop,
                zonal_demands_per_crop,
                current_zonal_production_per_crop,
                irrigated_crop_fractions);
        }

    }  // namespace value_policies
}  // namespace lue
