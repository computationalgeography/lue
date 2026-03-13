#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/concept.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    template<typename Policies, Rank rank>
        requires(
            std::is_floating_point_v<policy::OutputElementT<Policies, 0>> &&
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>> &&
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 1>>)
    auto open_simplex_noise(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& x_coordinates,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& y_coordinates,
        int seed) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
