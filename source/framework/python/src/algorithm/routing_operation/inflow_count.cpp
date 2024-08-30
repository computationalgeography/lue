#include "lue/framework/algorithm/inflow_count.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<typename CountElement>
        PartitionedArray<CountElement, rank> inflow_count(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        {
            using Policies = policy::inflow_count::DefaultValuePolicies<CountElement, FlowDirectionElement>;

            return inflow_count<CountElement>(Policies{}, flow_direction);
        }

    }  // Anonymous namespace


    void bind_inflow_count(pybind11::module& module)
    {
        module.def("inflow_count", inflow_count<std::uint8_t>);
    }

}  // namespace lue::framework
