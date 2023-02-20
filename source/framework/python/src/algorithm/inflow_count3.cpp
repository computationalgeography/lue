#include "lue/framework/algorithm/inflow_count3.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<typename CountElement>
        PartitionedArray<CountElement, rank> inflow_count3(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        {
            using Policies = policy::inflow_count3::DefaultValuePolicies<CountElement, FlowDirectionElement>;

            return inflow_count3<CountElement>(Policies{}, flow_direction);
        }

    }  // Anonymous namespace


    void bind_inflow_count3(pybind11::module& module)
    {
        module.def("inflow_count3", inflow_count3<std::uint8_t>);
    }

}  // namespace lue::framework
