#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<typename Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "kinematic_wave",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<Element, rank> const& discharge,
                           PartitionedArray<Element, rank> const& inflow,
                           Element const alpha,
                           Element const beta,
                           Element const time_step_duration,
                           PartitionedArray<Element, rank> const& channel_length)
                        {
                            return value_policies::kinematic_wave(
                                flow_direction,
                                discharge,
                                inflow,
                                alpha,
                                beta,
                                time_step_duration,
                                channel_length);
                        });
                }
        };

    }  // Anonymous namespace


    void bind_kinematic_wave(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
