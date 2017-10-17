#include "lue/constant_size/time/located/property_set.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {

void init_property_set(
    py::module& module)
{

    py::class_<PropertySet, constant_size::PropertySet>(
        module,
        "PropertySet",
        "PropertySet docstring...")

        ;

}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
