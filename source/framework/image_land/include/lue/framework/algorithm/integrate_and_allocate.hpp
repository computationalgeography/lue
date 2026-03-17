#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework/serial_route.hpp"
#include <functional>
#include <map>
#include <tuple>
#include <vector>


namespace lue {
    // Inputs:
    // 0. ZoneElement: route ID
    // 1. FloatingPointElement: sdp_factor
    // 2. FloatingPointElement: yield_factor
    // 3. FloatingPointElement: crop_fraction
    // 4. FloatingPointElement: demand
    // 5. FloatingPointElement: production
    // 6. FloatingPointElement: irrigated_crop_fraction

    // Outputs:
    // 0. FloatingPointElement: crop_fraction
    // 1. ...
    // 2. ...

    template<typename Policies, Rank rank>
    auto integrate_and_allocate(

        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& routes,

        // [0, ~10] max depends on units
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 1>, rank> const>> const&
            sdp_factors_per_crop,

        // [≥ 0]
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 2>, rank> const>> const&
            yield_factors_per_crop,

        // [0, 1]
        // TODO Rename to current
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 3>, rank> const>> const&
            initial_crop_fractions_per_crop,

        // demand: ≥ 0
        hpx::shared_future<std::map<
            policy::InputElementT<Policies, 0>,
            std::vector<policy::InputElementT<Policies, 4>>>> const& zonal_demands_per_crop,

        // Productions are not known yet
        // production: ≥ 0
        hpx::shared_future<std::map<
            policy::InputElementT<Policies, 0>,
            std::vector<policy::InputElementT<Policies, 5>>>> const& current_zonal_production_per_crop,

        // [0, 1]
        PartitionedArray<policy::InputElementT<Policies, 6>, rank> const& irrigated_crop_fractions)

        -> std::tuple<

            // Updated crop fractions per crop
            std::vector<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>,

            // Zonal production per crop
            hpx::shared_future<std::map<
                policy::InputElementT<Policies, 0>,
                std::vector<policy::OutputElementT<Policies, 1>>>>>;

}  // namespace lue
