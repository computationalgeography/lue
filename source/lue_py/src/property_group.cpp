#include "lue/info/property/property_group.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_property_group(
    py::module& module)
{

    py::class_<PropertyGroup, hdf5::Group>(
        module,
        "PropertyGroup",
        R"(
    PropertyGroup docstring...
)")

        ;

}

}  // namespace lue
