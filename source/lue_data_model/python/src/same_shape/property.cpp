#include "lue/info/property/same_shape/property.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace same_shape {

void init_property(
    py::module& module)
{

    py::class_<Property, PropertyGroup>(
        module,
        "Property",
        R"(
    Property docstring...
)")

        .def_property_readonly("value",
            py::overload_cast<>(&Property::value),
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace same_shape
}  // namespace lue
