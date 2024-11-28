#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/define.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <tuple>


namespace lue::detail {

    template<typename FlowDirectionData>
    auto downstream_cell(
        FlowDirectionData const& flow_direction_data,
        Count nr_elements0,
        Count nr_elements1,
        Index const idx0,
        Index const idx1,
        Index& offset0,
        Index& offset1) -> bool
    {
        using FlowDirectionElement = ElementT<FlowDirectionData>;

        FlowDirectionElement const flow_direction{flow_direction_data(idx0, idx1)};

        offset0 = 0;
        offset1 = 0;
        bool is_output_cell{false};  // Default is OK for sinks

        if (flow_direction == north_west<FlowDirectionElement>)
        {
            offset0 = -1;
            offset1 = -1;
            is_output_cell = idx0 == 0 || idx1 == 0;
        }
        else if (flow_direction == north<FlowDirectionElement>)
        {
            offset0 = -1;
            is_output_cell = idx0 == 0;
        }
        else if (flow_direction == north_east<FlowDirectionElement>)
        {
            offset0 = -1;
            offset1 = 1;
            is_output_cell = idx0 == 0 || idx1 == nr_elements1 - 1;
        }
        else if (flow_direction == west<FlowDirectionElement>)
        {
            offset1 = -1;
            is_output_cell = idx1 == 0;
        }
        else if (flow_direction == east<FlowDirectionElement>)
        {
            offset1 = 1;
            is_output_cell = idx1 == nr_elements1 - 1;
        }
        else if (flow_direction == south_west<FlowDirectionElement>)
        {
            offset0 = 1;
            offset1 = -1;
            is_output_cell = idx0 == nr_elements0 - 1 || idx1 == 0;
        }
        else if (flow_direction == south<FlowDirectionElement>)
        {
            offset0 = 1;
            is_output_cell = idx0 == nr_elements0 - 1;
        }
        else if (flow_direction == south_east<FlowDirectionElement>)
        {
            offset0 = 1;
            offset1 = 1;
            is_output_cell = idx0 == nr_elements0 - 1 || idx1 == nr_elements1 - 1;
        }

        return !is_output_cell;
    }


    template<typename FlowDirectionData>
    auto downstream_cell(FlowDirectionData const& flow_direction_data, Index const idx0, Index const idx1)
        -> std::tuple<Index, Index>
    {
        using FlowDirectionElement = ElementT<FlowDirectionData>;

        FlowDirectionElement const flow_direction{flow_direction_data(idx0, idx1)};

        Index offset0{0};
        Index offset1{0};

        if (flow_direction == north_west<FlowDirectionElement>)
        {
            offset0 = -1;
            offset1 = -1;
        }
        else if (flow_direction == north<FlowDirectionElement>)
        {
            offset0 = -1;
        }
        else if (flow_direction == north_east<FlowDirectionElement>)
        {
            offset0 = -1;
            offset1 = 1;
        }
        else if (flow_direction == west<FlowDirectionElement>)
        {
            offset1 = -1;
        }
        else if (flow_direction == east<FlowDirectionElement>)
        {
            offset1 = 1;
        }
        else if (flow_direction == south_west<FlowDirectionElement>)
        {
            offset0 = 1;
            offset1 = -1;
        }
        else if (flow_direction == south<FlowDirectionElement>)
        {
            offset0 = 1;
        }
        else if (flow_direction == south_east<FlowDirectionElement>)
        {
            offset0 = 1;
            offset1 = 1;
        }

        return {idx0 + offset0, idx1 + offset1};
    }

}  // namespace lue::detail
