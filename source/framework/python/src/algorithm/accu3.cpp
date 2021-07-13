#include "lue/framework/algorithm/accu3.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<
            typename MaterialElement>
        PartitionedArray<MaterialElement, rank> accu3(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& material)
        {
            using Policies = policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu3(Policies{}, flow_direction, material);
        }

    }  // Anonymous namespace


    void bind_accu3(
        pybind11::module& module)
    {
        module.def("accu3", accu3<float>);
        module.def("accu3", accu3<double>);
    }

}  // namespace lue::framework
