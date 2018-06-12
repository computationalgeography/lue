#include "lue/constant_collection/property.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_collection {

void init_property_class(
    py::module& module)
{

    py::class_<Property, lue::Property>(
        module,
        "Property",
        "Property docstring...")

        ;

}

}  // namespace constant_collection
}  // namespace lue
