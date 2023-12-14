#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"


namespace lue {

    template<typename Policies, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies, 0>, rank> highest_n(
        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& route,
        Count const max_nr_cells);

    template<typename Policies, typename FieldElement, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies, 0>, rank> highest_n(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone,
        PartitionedArray<FieldElement, rank> const& field,
        Count const max_nr_cells);

    template<typename Policies, typename FieldElement, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies, 0>, rank> highest_n(
        Policies const& policies,
        PartitionedArray<FieldElement, rank> const& field,
        Count const max_nr_cells);

}  // namespace lue
