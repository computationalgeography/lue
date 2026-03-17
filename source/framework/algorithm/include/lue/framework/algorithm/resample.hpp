#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    enum class DownscaleStrategy : std::uint8_t { assign };

    template<typename Policies, Rank rank>
    auto downscale(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& array,
        Count count,
        DownscaleStrategy strategy) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
