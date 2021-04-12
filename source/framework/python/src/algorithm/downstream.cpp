#include "lue/framework/algorithm/downstream.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<
            typename MaterialElement>
        PartitionedArray<MaterialElement, rank> downstream(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& material)
        {
            using Policies = policy::downstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return downstream(Policies{}, flow_direction, material);
        }

    }  // Anonymous namespace


    void bind_downstream(
        pybind11::module& module)
    {
        module.def("downstream", downstream<std::uint64_t>);
        module.def("downstream", downstream<float>);
        module.def("downstream", downstream<double>);
    }

}  // namespace lue::framework
