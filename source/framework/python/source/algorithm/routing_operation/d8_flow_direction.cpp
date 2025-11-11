#include "lue/framework/algorithm/value_policies/d8_flow_direction.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


using namespace pybind11::literals;


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
                        [](PartitionedArray<ElevationElement, rank> const& elevation) -> auto
                        { return value_policies::d8_flow_direction<FlowDirectionElement>(elevation); },
                        "elevation"_a);
                }
        };

    }  // Anonymous namespace


    void bind_d8_flow_direction(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
