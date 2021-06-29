#include "lue/framework/algorithm/accu_threshold3.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<
            typename MaterialElement>
        std::tuple<PartitionedArray<MaterialElement, rank>, PartitionedArray<MaterialElement, rank>>
            accu_threshold3(
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<MaterialElement, rank> const& material,
                PartitionedArray<MaterialElement, rank> const& threshold)
        {
            using Policies =
                policy::accu_threshold3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold3(Policies{}, flow_direction, material, threshold);
        }

    }  // Anonymous namespace


    void bind_accu_threshold3(
        pybind11::module& module)
    {
        module.def("accu_threshold3", accu_threshold3<float>);
        module.def("accu_threshold3", accu_threshold3<double>);
    }

}  // namespace lue::framework
