#include "lue/framework/algorithm/value_policies/accu_threshold.hpp"
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
                        "accu_threshold",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& inflow,
                           PartitionedArray<FloatingPointElement, rank> const& threshold)
                        { return value_policies::accu_threshold(flow_direction, inflow, threshold); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "threshold"_a);
                    module.def(
                        "accu_threshold",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& inflow,
                           Scalar<FloatingPointElement> const& threshold)
                        { return value_policies::accu_threshold(flow_direction, inflow, threshold); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "threshold"_a);
                    module.def(
                        "accu_threshold",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           Scalar<FloatingPointElement> const& inflow,
                           Scalar<FloatingPointElement> const& threshold)
                        { return value_policies::accu_threshold(flow_direction, inflow, threshold); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "threshold"_a);
                    module.def(
                        "accu_threshold",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           Scalar<FloatingPointElement> const& inflow,
                           PartitionedArray<FloatingPointElement, rank> const& threshold)
                        { return value_policies::accu_threshold(flow_direction, inflow, threshold); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "threshold"_a);
                }
        };

    }  // Anonymous namespace


    void bind_accu_threshold(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
