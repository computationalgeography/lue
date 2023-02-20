#include "lue/framework/algorithm/upstream.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<typename MaterialElement>
        PartitionedArray<MaterialElement, rank> upstream(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& material)
        {
            using Policies = policy::upstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return upstream(Policies{}, flow_direction, material);
        }

    }  // Anonymous namespace


    void bind_upstream(pybind11::module& module)
    {
        module.def("upstream", upstream<std::uint64_t>);
        module.def("upstream", upstream<float>);
        module.def("upstream", upstream<double>);
    }

}  // namespace lue::framework
