#include "lue/framework/algorithm/value_policies/accu_capacity.hpp"
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
                        "accu_capacity",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& inflow,
                           PartitionedArray<FloatingPointElement, rank> const& capacity)
                        { return value_policies::accu_capacity(flow_direction, inflow, capacity); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "capacity"_a);
                    module.def(
                        "accu_capacity",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<FloatingPointElement, rank> const& inflow,
                           Scalar<FloatingPointElement> const& capacity)
                        { return value_policies::accu_capacity(flow_direction, inflow, capacity); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "capacity"_a);
                    module.def(
                        "accu_capacity",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           Scalar<FloatingPointElement> const& inflow,
                           Scalar<FloatingPointElement> const& capacity)
                        { return value_policies::accu_capacity(flow_direction, inflow, capacity); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "capacity"_a);
                    module.def(
                        "accu_capacity",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           Scalar<FloatingPointElement> const& inflow,
                           PartitionedArray<FloatingPointElement, rank> const& capacity)
                        { return value_policies::accu_capacity(flow_direction, inflow, capacity); },
                        "flow_direction"_a,
                        "inflow"_a,
                        "capacity"_a);
                }
        };

    }  // Anonymous namespace


    void bind_accu_capacity(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
