#include "lue/framework/algorithm/value_policies/accu_info.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {

    void bind_accu_info(pybind11::module& module)
    {
        Rank const rank{2};

        module.def(
            "accu_info",
            [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
            { return value_policies::accu_info(flow_direction); },
            "flow_direction"_a);
    }

}  // namespace lue::framework
