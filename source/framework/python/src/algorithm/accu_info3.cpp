#include "lue/framework/algorithm/accu_info3.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;


        PartitionedArray<CellClass, rank> accu_info3(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        {
            using Policies = policy::accu_info3::DefaultValuePolicies<FlowDirectionElement>;

            return lue::accu_info3(Policies{}, flow_direction);
        }

    }  // Anonymous namespace


    void bind_accu_info3(
        pybind11::module& module)
    {
        module.def("accu_info3", accu_info3);
    }

}  // namespace lue::framework
