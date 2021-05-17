#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue::test {

    using FlowDirectionElement = std::uint32_t;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirectionElement, 2>;

    ShapeT<FlowDirectionArray> const array_shape{{9, 9}};
    ShapeT<FlowDirectionArray> const partition_shape{{3, 3}};

    auto const n{lue::north<FlowDirectionElement>};
    auto const ne{lue::north_east<FlowDirectionElement>};
    auto const e{lue::east<FlowDirectionElement>};
    auto const se{lue::south_east<FlowDirectionElement>};
    auto const s{lue::south<FlowDirectionElement>};
    auto const sw{lue::south_west<FlowDirectionElement>};
    auto const w{lue::west<FlowDirectionElement>};
    auto const nw{lue::north_west<FlowDirectionElement>};
    auto const p{lue::sink<FlowDirectionElement>};

    auto const nd{lue::no_data<FlowDirectionElement>};


    FlowDirectionArray all_no_data();

    FlowDirectionArray spiral_in();

    FlowDirectionArray merging_streams();

    FlowDirectionArray pcraster_example_flow_direction();

    template<
        typename Element>
    PartitionedArray<Element, 2> filled(Element const fill_value);


    template<
        typename Element>
    PartitionedArray<Element, 2> ones()
    {
        return filled(Element{1});
    }


    template<
        typename Element>
    PartitionedArray<Element, 2> zeros()
    {
        return filled(Element{0});
    }


    template<
        typename Element>
    PartitionedArray<Element, 2> no_data()
    {
        return filled(policy::no_data_value<Element>);
    }

}  // namespace lue::test
