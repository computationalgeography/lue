#include "lue/constant_size/property.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {

void init_property_class(
    py::module& module)
{

    py::class_<Property, lue::Property>(
        module,
        "Property",
        "Property docstring...")

        ;

}

}  // namespace constant_size
}  // namespace lue
