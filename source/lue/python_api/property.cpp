#include "lue/python_api/collection.h"
#include "lue/cxx_api/properties.h"
// #include "lue/cxx_api/property_set_api.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_property(
        py::module& module)
{

    // py::class_<api::Property>(module, "_Property",
    //     "_Property docstring...")

    //     // Property API
    //     .def_property_readonly("id", &api::Property::id,
    //         "id docstring...",
    //         py::return_value_policy::reference_internal)
    // ;

    // py::class_<api::omnipresent::omnipresent::Property>(module,
    //     "O_O_Property", py::base<api::Property>(),
    //     "O_O_Property docstring...")
    //     .def(py::init<Property&, hid_t const>(),
    //         "__init__ docstring..."
    //         "group"_a, "type"_a,
    //         py::keep_alive<1, 2>())
    //     .def("reserve_items",
    //             &api::omnipresent::omnipresent::Property::reserve_items,
    //         "reserve docstring...",
    //         py::return_value_policy::reference_internal)
    //     .def_property_readonly("items",
    //             &api::omnipresent::omnipresent::Property::items,
    //         "items docstring...",
    //         py::return_value_policy::reference_internal)
    // ;

    // BASE_COLLECTION(Property)

    // py::class_<Properties>(module, "Properties", py::base<PropertyCollection>(),
    //     "Properties docstring...")
    //     .def("add", &Properties::add,
    //         "add docstring...",
    //         py::return_value_policy::reference_internal)
    // ;

    py::class_<Property>(module, "Property", py::base<hdf5::Group>(),
        "Property docstring...")
    ;

}

}  // namespace python
}  // namespace lue
