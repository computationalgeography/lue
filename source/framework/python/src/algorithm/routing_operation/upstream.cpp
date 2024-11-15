#include "lue/framework/algorithm/upstream.hpp"
#include "bind.hpp"
#include "lue/framework/configure.hpp"


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
                        "upstream",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<MaterialElement, rank> const& material)
                        {
                            using Policies =
                                policy::upstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

                            return upstream(Policies{}, flow_direction, material);
                        });
                }
        };

    }  // Anonymous namespace


    void bind_upstream(pybind11::module& module)
    {
        bind<Binder, MaterialElements>(module);
    }

}  // namespace lue::framework
