#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<
            typename Element>
        PartitionedArray<Element, rank> kinematic_wave(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<Element, rank> const& discharge,
            PartitionedArray<Element, rank> const& inflow,
            Element const alpha,
            Element const beta,
            Element const time_step_duration,
            PartitionedArray<Element, rank> const& channel_length)
        {
            return value_policies::kinematic_wave(
                flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length);
        }

    }  // Anonymous namespace


    void bind_kinematic_wave(
        pybind11::module& module)
    {
        module.def("kinematic_wave", kinematic_wave<float>);
        module.def("kinematic_wave", kinematic_wave<double>);
    }

}  // namespace lue::framework
