#include "collection.h"
#include "lue/properties.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_property_class(
        py::module& module)
{

    BASE_COLLECTION(Properties, Property)

    py::class_<Properties, PropertyCollection>(
        module,
        "Properties",
        R"(
    Properties docstring...
)")







        // .def("add", &Properties<>::add,
        //     "add docstring...",
        //     py::return_value_policy::reference_internal)

        ;

    py::class_<Property, hdf5::Group>(
        module,
        "Property",
        "Property docstring...")

        .def("__repr__",
            [](Property const& property) {
                return "Property(pathname='" +
                    property.id().pathname() + "')";
            }
        )

        .def_property_readonly(
            "name",
            [](Property const& self)
            {
                return self.id().name();
            },
            "name docstring...")

        .def("discretize_space",
            &Property::discretize_space,
            "discretize_space docstring...",
            "property"_a)

        .def_property_readonly(
            "time_is_discretized",
            &Property::time_is_discretized,
            "time_is_discretized docstring...")

        .def_property_readonly(
            "space_is_discretized",
            &Property::space_is_discretized,
            "space_is_discretized docstring...")

        ;

}

}  // namespace lue
