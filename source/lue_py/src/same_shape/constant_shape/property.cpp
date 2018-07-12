#include "lue/info/property/same_shape/constant_shape/property.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace same_shape {
namespace constant_shape {

void init_property(
    py::module& module)
{

    py::class_<Property, PropertyGroup>(
        module,
        "Property",
        R"(
    Property docstring...
)")

        ;

}

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
