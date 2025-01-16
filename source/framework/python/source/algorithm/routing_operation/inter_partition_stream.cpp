#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_inter_partition_stream(pybind11::module& module)
    {
        Rank const rank{2};

        module.def(
            "inter_partition_stream",
            [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
                -> PartitionedArray<StreamClass, rank>
            {
                using Policies = policy::inter_partition_stream::DefaultValuePolicies<FlowDirectionElement>;

                return std::get<0>(inter_partition_stream(Policies{}, flow_direction));
            });
    }

}  // namespace lue::framework
