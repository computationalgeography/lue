#define BOOST_TEST_MODULE lue framework api cxx routing accu
#include "lue/framework/api/cxx/routing/accu.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(accu_raster_raster)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using FloatingPointElement = lue::FloatingPointElement<0>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirectionElement, 2>;
    using FloatingPointArray = lue::PartitionedArray<FloatingPointElement, 2>;

    lue::api::Field const flow_direction = FlowDirectionArray{};
    lue::api::Field const inflow = FloatingPointArray{};
    lue::api::Field outflow = lue::api::accu(flow_direction, inflow);

    BOOST_CHECK(std::holds_alternative<FloatingPointArray>(outflow.variant()));
}


BOOST_AUTO_TEST_CASE(accu_raster_scalar)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using FloatingPointElement = lue::FloatingPointElement<0>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirectionElement, 2>;
    using FloatingPointArray = lue::PartitionedArray<FloatingPointElement, 2>;
    using FloatingPointScalar = lue::Scalar<FloatingPointElement>;

    lue::api::Field const flow_direction = FlowDirectionArray{};
    lue::api::Field const inflow = FloatingPointScalar{};
    lue::api::Field outflow = lue::api::accu(flow_direction, inflow);

    BOOST_CHECK(std::holds_alternative<FloatingPointArray>(outflow.variant()));
}
