#include "lue/framework/algorithm/accu_threshold.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<typename MaterialElement>
        std::tuple<PartitionedArray<MaterialElement, rank>, PartitionedArray<MaterialElement, rank>>
        accu_threshold(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            PartitionedArray<MaterialElement, rank> const& material,
            PartitionedArray<MaterialElement, rank> const& threshold)
        {
            using Policies =
                policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold(Policies{}, flow_direction, material, threshold);
        }

    }  // Anonymous namespace


    void bind_accu_threshold(pybind11::module& module)
    {
        module.def("accu_threshold", accu_threshold<float>);
        module.def("accu_threshold", accu_threshold<double>);
    }

}  // namespace lue::framework
