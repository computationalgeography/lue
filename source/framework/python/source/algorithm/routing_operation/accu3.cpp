#include "lue/framework/algorithm/value_policies/accu3.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<Arithmetic MaterialElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "accu3",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<MaterialElement, rank> const& material)
                        { return value_policies::accu3(flow_direction, material); });
                }
        };

    }  // Anonymous namespace


    void bind_accu3(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
