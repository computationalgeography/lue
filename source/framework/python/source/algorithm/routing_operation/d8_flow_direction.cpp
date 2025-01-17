#include "lue/framework/algorithm/d8_flow_direction.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<std::floating_point ElevationElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "d8_flow_direction",
                        [](PartitionedArray<ElevationElement, rank> const& elevation)
                        {
                            using Policies = policy::d8_flow_direction::
                                DefaultValuePolicies<FlowDirectionElement, ElevationElement>;

                            return d8_flow_direction<FlowDirectionElement>(Policies{}, elevation);
                        });
                }
        };

    }  // Anonymous namespace


    void bind_d8_flow_direction(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
