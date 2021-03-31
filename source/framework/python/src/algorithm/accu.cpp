#include "lue/framework/algorithm/accu.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<
            typename MaterialElement>
        PartitionedArray<MaterialElement, rank> accu(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& material)
        {
            using Policies = policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu(Policies{}, flow_direction, material);
        }

    }  // Anonymous namespace


    void bind_accu(
        pybind11::module& module)
    {
        module.def("accu", accu<std::uint64_t>);
        module.def("accu", accu<float>);
        module.def("accu", accu<double>);
    }

}  // namespace lue::framework
