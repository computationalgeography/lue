#include "lue/framework/algorithm/sqrt.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;


namespace lue {
namespace framework {

void bind_local_operations(
    py::module& module)
{
    module.def(
        "sqrt",
        [](
            PartitionedArray<double, 2> const& array)
        {
            return sqrt(array);
        },
        py::return_value_policy::move);

    module.def(
        "uniform",
        [](
            PartitionedArray<double, 2> const& array,
            double const min,
            double const max)
        {
            return uniform(array, min, max);
        },
        py::return_value_policy::move);

}

}  // namespace framework
}  // namespace lue
