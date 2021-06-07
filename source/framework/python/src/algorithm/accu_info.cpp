#include "lue/framework/algorithm/accu_info.hpp"
#include <pybind11/stl.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;


        std::tuple<
                PartitionedArray<CellClass, rank>,
                hpx::shared_future<std::vector<PartitionedArray<PartitionClass, rank>>>
            >
            accu_info(
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        {
            using Policies = policy::accu_info::DefaultValuePolicies<FlowDirectionElement>;

            return lue::accu_info(Policies{}, flow_direction);
        }

    }  // Anonymous namespace


    void bind_accu_info(
        pybind11::module& module)
    {
        module.def("accu_info", accu_info);
    }

}  // namespace lue::framework
