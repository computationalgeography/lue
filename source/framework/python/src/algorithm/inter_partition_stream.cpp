#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        PartitionedArray<StreamClass, rank> inter_partition_stream(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
        {
            using Policies = policy::inter_partition_stream::DefaultValuePolicies<FlowDirectionElement>;

            return std::get<0>(inter_partition_stream(Policies{}, flow_direction));
        }

    }  // Anonymous namespace


    void bind_inter_partition_stream(pybind11::module& module)
    {
        module.def("inter_partition_stream", inter_partition_stream);
    }

}  // namespace lue::framework
