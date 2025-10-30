#include "lue/framework/algorithm/value_policies/partial_accu.hpp"
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
                        "partial_accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           Scalar<FloatingPointElement> const& inflow,
                           Count const nr_steps) -> auto
                        { return value_policies::partial_accu(flow_direction, inflow, nr_steps); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "nr_steps"_a);
                    module.def(
                        "partial_accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           FloatingPointElement const& inflow,
                           Count const nr_steps) -> auto
                        {
                            return value_policies::partial_accu(
                                flow_direction, lue::Scalar{inflow}, nr_steps);
                        },
                        "flow_direction"_a,
                        "inflow"_a,
                        "nr_steps"_a);
                }
        };

    }  // Anonymous namespace


    void bind_partial_accu(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
