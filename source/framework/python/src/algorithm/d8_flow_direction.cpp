#include "lue/framework/algorithm/d8_flow_direction.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<
            typename ElevationElement>
        PartitionedArray<FlowDirectionElement, rank> d8_flow_direction(
            PartitionedArray<ElevationElement, rank> const& elevation)
        {
            using Policies =
                policy::d8_flow_direction::DefaultValuePolicies<FlowDirectionElement, ElevationElement>;

            return d8_flow_direction<FlowDirectionElement>(Policies{}, elevation);
        }

    }  // Anonymous namespace


    void bind_d8_flow_direction(
        pybind11::module& module)
    {
        module.def("d8_flow_direction", d8_flow_direction<float>);
        module.def("d8_flow_direction", d8_flow_direction<double>);
    }

}  // namespace lue::framework
