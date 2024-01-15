#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"
#include <functional>
#include <map>
#include <tuple>
#include <vector>


namespace lue {

    template<typename Policies, Rank rank>
    auto integrate_and_allocate(
        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& route,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& zone,
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 2>, rank> const>> const&
            crop_fractions)
        -> std::tuple<
            // Crop fractions
            std::vector<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>,
            // Zonal production
            std::map<policy::InputElementT<Policies, 1>, policy::OutputElementT<Policies, 1>>>;

}  // namespace lue
