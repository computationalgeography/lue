#include "lue/framework/algorithm/accu_fraction.hpp"
#include "lue/framework/configure.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "accu_fraction",
                        [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                           PartitionedArray<Element, rank> const& material,
                           PartitionedArray<Element, rank> const& fraction)
                        {
                            using Policies = policy::accu_fraction::
                                DefaultValuePolicies<FlowDirectionElement, Element, Element>;

                            return accu_fraction(Policies{}, flow_direction, material, fraction);
                        });
                }
        };

    }  // Anonymous namespace


    void bind_accu_fraction(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework
