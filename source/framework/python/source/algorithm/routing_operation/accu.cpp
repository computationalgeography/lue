#include "lue/framework/algorithm/value_policies/accu.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        using namespace pybind11::literals;

        class Binder
        {

            public:

                template<Arithmetic MaterialElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<MaterialElement, rank> const& material)
                        { return value_policies::accu(flow_direction, material); },
                        "flow_direction"_a,
                        "material"_a);
                    module.def(
                        "accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           Scalar<MaterialElement> const& material)
                        { return value_policies::accu(flow_direction, material); },
                        "flow_direction"_a,
                        "material"_a);
                    module.def(
                        "accu",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           MaterialElement const& material)
                        { return value_policies::accu(flow_direction, material); },
                        "flow_direction"_a,
                        "material"_a);
                }
        };

    }  // Anonymous namespace


    void bind_accu(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
