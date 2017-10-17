#include "lue/constant_size/property_set.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
// using namespace pybind11::literals;


namespace lue {
namespace constant_size {

void init_property_set_class(
    py::module& module)
{

    py::class_<PropertySet, lue::PropertySet>(
        module,
        "PropertySet",
        "PropertySet docstring...")

        // .def(
        //     py::init<lue::Phenomenon&, std::string const&>(),
        //     "__init__ docstring..."
        //     "phenomenon"_a,
        //     "name"_a,
        //     py::keep_alive<1, 2>())

        ;

}

}  // namespace constant_size
}  // namespace lue
