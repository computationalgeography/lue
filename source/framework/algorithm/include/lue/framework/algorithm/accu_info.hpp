#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework.hpp"


namespace lue {

    using CellClass = SmallestUnsignedIntegralElement;
    static constexpr CellClass ridge_cell = 11;
    static constexpr CellClass intra_partition_stream_cell = 12;
    static constexpr CellClass partition_output_cell = 13;
    static constexpr CellClass partition_input_cell = 14;
    static constexpr CellClass partition_input_output_cell = 15;
    static constexpr CellClass inter_partition_stream_cell = 16;
    static constexpr CellClass sink_cell = 17;


    template<typename Policies>
    auto accu_info(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

}  // namespace lue
