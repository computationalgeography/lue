#define BOOST_TEST_MODULE lue framework api cxx routing d8_flow_direction
#include "unit_test.hpp"
#include "lue/framework/api/cxx/routing/d8_flow_direction.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(d8_flow_direction)
{
    using lue::FlowDirectionElement;
    using ElevationElement = lue::FloatingPointElement<0>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirectionElement, 2>;
    using ElevationArray = lue::PartitionedArray<ElevationElement, 2>;

    lue::api::Field const elevation = ElevationArray{};
    lue::api::Field flow_direction = lue::api::d8_flow_direction(elevation);

    BOOST_CHECK(std::holds_alternative<FlowDirectionArray>(flow_direction.variant()));
}
