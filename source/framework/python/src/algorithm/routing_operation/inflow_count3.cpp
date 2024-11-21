#include "lue/framework/algorithm/inflow_count3.hpp"
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
                        "inflow_count3",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
                        {
                            using Policies = policy::inflow_count3::
                                DefaultValuePolicies<CountElement, FlowDirectionElement>;

                            return inflow_count3<CountElement>(Policies{}, flow_direction);
                        });
                }
        };

    }  // Anonymous namespace


    void bind_inflow_count3(pybind11::module& module)
    {
        bind<Binder, SmallestIntegralElement>(module);
    }

}  // namespace lue::framework
