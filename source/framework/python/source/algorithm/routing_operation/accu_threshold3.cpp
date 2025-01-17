#include "lue/framework/algorithm/value_policies/accu_threshold3.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<std::floating_point MaterialElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "accu_threshold3",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<MaterialElement, rank> const& material,
                           PartitionedArray<MaterialElement, rank> const& threshold)
                        { return value_policies::accu_threshold3(flow_direction, material, threshold); });
                }
        };

    }  // Anonymous namespace


    void bind_accu_threshold3(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
