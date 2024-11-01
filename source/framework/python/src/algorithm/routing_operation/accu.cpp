#include "lue/framework/algorithm/accu.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<Arithmetic MaterialElement>
        void bind(pybind11::module& module)
        {
            Rank const rank{2};

            module.def(
                "accu",
                [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                   PartitionedArray<MaterialElement, rank> const& material)
                {
                    using Policies =
                        policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

                    return accu(Policies{}, flow_direction, material);
                });
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx == 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx > 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
            bind<Elements, idx - 1>(module);
        }


        template<TupleLike Elements>
        void bind(pybind11::module& module)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1>(module);
        }

    }  // Anonymous namespace


    void bind_accu(pybind11::module& module)
    {
        bind<MaterialElements>(module);
    }

}  // namespace lue::framework
