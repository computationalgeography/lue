#include "lue/framework/algorithm/downstream_distance.hpp"
#include <pybind11/numpy.h>
#include <fmt/format.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        // template<
        //     typename DistanceElement>
        // PartitionedArray<DistanceElement, rank> downstream_distance(
        //     PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        //     DistanceElement const cell_size)
        // {
        //     using Policies =
        //         policy::downstream_distance::DefaultValuePolicies<FlowDirectionElement, DistanceElement>;

        //     return downstream_distance(Policies{}, flow_direction, cell_size);
        // }


        template<
            typename DistanceElement>
        pybind11::object downstream_distance(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            pybind11::object const& cell_size)
        {
            using Policies =
                policy::downstream_distance::DefaultValuePolicies<FlowDirectionElement, DistanceElement>;

            return pybind11::cast(
                downstream_distance(Policies{}, flow_direction,
                        pybind11::cast<DistanceElement>(cell_size)));
        }

    }  // Anonymous namespace


    void bind_downstream_distance(
        pybind11::module& module)
    {
        // module.def("downstream_distance", downstream_distance<float>);
        // module.def("downstream_distance", downstream_distance<double>);

        module.def(
            "downstream_distance",
            [](
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                pybind11::object const& cell_size,
                pybind11::object const& dtype_args)
            {
                pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                auto const kind = dtype.kind();
                auto const size = dtype.itemsize();  // bytes

                if(kind != 'f' || (size != 4 && size != 8))
                {
                    throw std::runtime_error(
                        fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
                }

                pybind11::object result;

                if(size == 4)
                {
                    result = downstream_distance<float>(flow_direction, cell_size);
                }
                else if(size == 8)
                {
                    result = downstream_distance<double>(flow_direction, cell_size);
                }

                lue_hpx_assert(result);

                return result;
            }
        );
    }

}  // namespace lue::framework
