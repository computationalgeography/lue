#pragma once
#include "lue/framework/algorithm/detail/flow_direction.hpp"
#include <vector>


namespace lue {
namespace detail {

template<
    typename Accumulator,
    typename Index,
    typename FlowDirectionData,
    typename InflowCountData>
void accumulate(
    Accumulator& accumulator,
    Index idx0,
    Index idx1,
    FlowDirectionData const& flow_direction_data,
    InflowCountData& inflow_count_data)
{
    auto const [nr_elements0, nr_elements1] = flow_direction_data.shape();
    Index offset0, offset1;
    bool is_within_partition;

    while(true)
    {
        lue_assert(inflow_count_data(idx0, idx1) == 0);

        is_within_partition = downstream_cell(
            flow_direction_data, nr_elements0, nr_elements1, idx0, idx1, offset0, offset1);

        if(!is_within_partition)
        {
            // Current cell is partition output cell → notify accumulator and stop
            accumulator.at_partition_output_cell(idx0, idx1);
            break;
        }

        if(offset0 == 0 && offset1 == 0)
        {
            // Current cell is a sink → stop
            break;
        }

        // Downstream cell is pointed to by idx0 + offset0 and idx1 + offset1

        // Update the inflow count of the downstream cell
        lue_assert(inflow_count_data(idx0 + offset0, idx1 + offset1) >= 1);
        --inflow_count_data(idx0 + offset0, idx1 + offset1);

        // Move material to the downstream cell
        accumulator.accumulate(idx0, idx1, idx0 + offset0, idx1 + offset1);

        // Prepare for next iteration
        idx0 += offset0;
        idx1 += offset1;

        if(inflow_count_data(idx0, idx1) > 0)
        {
            // There are other streams flowing into the new cell → stop
            break;
        }
    }
}

}  // namespace detail
}  // namespace lue
