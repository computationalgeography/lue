#include "lue/framework/algorithm/accu_info3.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_accu_info3(pybind11::module& module)
    {
        Rank const rank{2};

        module.def(
            "accu_info3",
            [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
            {
                using Policies = policy::accu_info3::DefaultValuePolicies<FlowDirectionElement>;

                return lue::accu_info3(Policies{}, flow_direction);
            });
    }

}  // namespace lue::framework
