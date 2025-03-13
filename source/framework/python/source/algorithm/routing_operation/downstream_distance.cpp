#include "lue/framework/algorithm/downstream_distance.hpp"
#include "lue/framework.hpp"
#include <pybind11/numpy.h>
#include <format>


namespace lue::framework {
    namespace {

        Rank const rank{2};


        template<typename DistanceElement>
        auto downstream_distance(
            PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
            pybind11::object const& cell_size) -> pybind11::object
        {
            using Policies =
                policy::downstream_distance::DefaultValuePolicies<FlowDirectionElement, DistanceElement>;

            return pybind11::cast(
                downstream_distance(Policies{}, flow_direction, pybind11::cast<DistanceElement>(cell_size)));
        }

    }  // Anonymous namespace


    void bind_downstream_distance(pybind11::module& module)
    {
        module.def(
            "downstream_distance",
            [](PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
               pybind11::object const& cell_size,
               pybind11::object const& dtype_args)
            {
                pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                auto const kind = dtype.kind();
                auto const size = dtype.itemsize();  // bytes
                pybind11::object result;

                if (size == 4)
                {
                    using Element = float;

                    if constexpr (arithmetic_element_supported<Element>)
                    {
                        result = downstream_distance<Element>(flow_direction, cell_size);
                    }
                }
                else if (size == 8)
                {
                    using Element = double;

                    if constexpr (arithmetic_element_supported<Element>)
                    {
                        result = downstream_distance<Element>(flow_direction, cell_size);
                    }
                }

                if (!result)
                {
                    throw std::runtime_error(
                        std::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
                }

                return result;
            });
    }

}  // namespace lue::framework
