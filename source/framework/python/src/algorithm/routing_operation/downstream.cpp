#include "lue/framework/algorithm/downstream.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<typename MaterialElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "downstream",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<MaterialElement, rank> const& material)
                        {
                            using Policies = policy::downstream::
                                DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

                            return downstream(Policies{}, flow_direction, material);
                        });
                }
        };

    }  // Anonymous namespace


    void bind_downstream(pybind11::module& module)
    {
        bind<Binder, MaterialElements>(module);
    }

}  // namespace lue::framework
