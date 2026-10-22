#include "lue/framework/algorithm/value_policies/inflow_count.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<typename CountElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "inflow_count",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
                        { return value_policies::inflow_count<CountElement>(flow_direction); });
                }
        };

    }  // Anonymous namespace


    void bind_inflow_count(pybind11::module& module)
    {
        // We're using the boolean element type for the counts, assuming the boolean element type is a small
        // integral capable of representing small numbers.
        bind<Binder, BooleanElement>(module);
    }

}  // namespace lue::framework
