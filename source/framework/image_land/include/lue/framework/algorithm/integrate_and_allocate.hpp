#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
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
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 1>, rank> const>> const&
            sdp_factors,
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 2>, rank> const>> const&
            yield_factors,
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 3>, rank> const>> const&
            crop_fractions,
        std::map<policy::InputElementT<Policies, 0>, std::vector<policy::InputElementT<Policies, 4>>> const&
            demands,
        std::map<policy::InputElementT<Policies, 0>, std::vector<policy::InputElementT<Policies, 5>>> const&
            current_production,
        PartitionedArray<policy::InputElementT<Policies, 6>, rank> const& irrigated_crop_fractions)
        -> std::tuple<
            // Crop fractions
            std::vector<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>,
            // Zonal production
            // Per zone and crop a single value
            std::map<policy::InputElementT<Policies, 0>, std::vector<policy::OutputElementT<Policies, 1>>>>;

}  // namespace lue
