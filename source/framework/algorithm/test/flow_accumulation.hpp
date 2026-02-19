#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/configure.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue::test {

    using FlowDirectionElement = FlowDirectionElement;
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

    auto const nd{lue::policy::no_data_value<FlowDirectionElement>};


    auto all_no_data() -> FlowDirectionArray;

    auto spiral_in() -> FlowDirectionArray;

    auto merging_streams() -> FlowDirectionArray;

    auto pcraster_example_flow_direction() -> FlowDirectionArray;


    template<typename Element>
    auto filled(Element fill_value) -> PartitionedArray<Element, 2>;


    template<typename Element>
    auto ones() -> PartitionedArray<Element, 2>
    {
        return filled(Element{1});
    }


    template<typename Element>
    auto zeros() -> PartitionedArray<Element, 2>
    {
        return filled(Element{0});
    }


    template<typename Element>
    auto no_data() -> PartitionedArray<Element, 2>
    {
        return filled(policy::no_data_value<Element>);
    }

}  // namespace lue::test
