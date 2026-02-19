#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    auto first_n(
        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& route,
        Count max_nr_cells) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
