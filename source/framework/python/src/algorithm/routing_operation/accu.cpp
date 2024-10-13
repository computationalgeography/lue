#include "lue/framework/algorithm/accu.hpp"
#include "lue/framework.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};

        template<typename MaterialElement>
        auto accu(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& material)
            -> PartitionedArray<MaterialElement, rank>
        {
            using Policies = policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, material);
        }


        template<typename Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx == 0)
        {
            module.def("accu", accu<std::tuple_element_t<idx, Elements>>);
        }


        template<typename Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx > 0)
        {
            module.def("accu", accu<std::tuple_element_t<idx, Elements>>);

            bind<Elements, idx - 1>(module);
        }


        template<typename Elements>
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
