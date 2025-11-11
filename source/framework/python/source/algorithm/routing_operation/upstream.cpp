#include "lue/framework/algorithm/value_policies/upstream.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


using namespace pybind11::literals;


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
                           PartitionedArray<MaterialElement, rank> const& material) -> auto
                        { return value_policies::upstream(flow_direction, material); },
                        "flow_direction"_a,
                        "material"_a);
                }
        };

    }  // Anonymous namespace


    void bind_upstream(pybind11::module& module)
    {
        bind<Binder, MaterialElements>(module);
    }

}  // namespace lue::framework
