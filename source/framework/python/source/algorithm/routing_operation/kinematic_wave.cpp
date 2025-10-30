#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<std::floating_point FloatingPointElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "kinematic_wave",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& current_outflow,
                           PartitionedArray<FloatingPointElement, rank> const& inflow,
                           PartitionedArray<FloatingPointElement, rank> const& alpha,
                           PartitionedArray<FloatingPointElement, rank> const& beta,
                           Scalar<FloatingPointElement> const& time_step_duration,
                           PartitionedArray<FloatingPointElement, rank> const& channel_length)
                        {
                            return value_policies::kinematic_wave(
                                flow_direction,
                                current_outflow,
                                inflow,
                                alpha,
                                beta,
                                time_step_duration,
                                channel_length);
                        },
                        "flow_direction"_a,
                        "current_outflow"_a,
                        "inflow"_a,
                        "alpha"_a,
                        "beta"_a,
                        "time_step_duration"_a,
                        "channel_length"_a);
                    module.def(
                        "kinematic_wave",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& current_outflow,
                           PartitionedArray<FloatingPointElement, rank> const& inflow,
                           Scalar<FloatingPointElement> const& alpha,
                           Scalar<FloatingPointElement> const& beta,
                           Scalar<FloatingPointElement> const& time_step_duration,
                           Scalar<FloatingPointElement> const& channel_length)
                        {
                            return value_policies::kinematic_wave(
                                flow_direction,
                                current_outflow,
                                inflow,
                                alpha,
                                beta,
                                time_step_duration,
                                channel_length);
                        },
                        "flow_direction"_a,
                        "current_outflow"_a,
                        "inflow"_a,
                        "alpha"_a,
                        "beta"_a,
                        "time_step_duration"_a,
                        "channel_length"_a);
                    module.def(
                        "kinematic_wave",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& current_outflow,
                           PartitionedArray<FloatingPointElement, rank> const& inflow,
                           FloatingPointElement const alpha,
                           FloatingPointElement const beta,
                           FloatingPointElement const time_step_duration,
                           FloatingPointElement const channel_length)
                        {
                            return value_policies::kinematic_wave(
                                flow_direction,
                                current_outflow,
                                inflow,
                                Scalar{alpha},
                                Scalar{beta},
                                Scalar{time_step_duration},
                                Scalar{channel_length});
                        },
                        "flow_direction"_a,
                        "current_outflow"_a,
                        "inflow"_a,
                        "alpha"_a,
                        "beta"_a,
                        "time_step_duration"_a,
                        "channel_length"_a);
                }
        };

    }  // Anonymous namespace


    void bind_kinematic_wave(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
