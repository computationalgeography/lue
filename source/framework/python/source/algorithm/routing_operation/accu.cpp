#include "lue/framework/algorithm/value_policies/accu.hpp"
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
                        "accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& inflow)
                        { return value_policies::accu(flow_direction, inflow); },
                        "flow_direction"_a,
                        "inflow"_a);
                    module.def(
                        "accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           Scalar<FloatingPointElement> const& inflow)
                        { return value_policies::accu(flow_direction, inflow); },
                        "flow_direction"_a,
                        "inflow"_a);
                    module.def(
                        "accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           FloatingPointElement const& inflow)
                        { return value_policies::accu(flow_direction, lue::Scalar{inflow}); },
                        "flow_direction"_a,
                        "inflow"_a);
                }
        };

    }  // Anonymous namespace


    void bind_accu(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
