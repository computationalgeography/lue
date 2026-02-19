#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    auto integrate(
        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& route,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& integrand,
        Count max_nr_cells) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;

}  // namespace lue
