#include "lue/cxx_api/phenomena.h"
#include "lue/python_api/collection.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_phenomenon(
        py::module& module)
{

    BASE_COLLECTION(Phenomenon)

    py::class_<Phenomena>(module, "Phenomena", py::base<PhenomenonCollection>(),
        "Phenomena docstring...")
        .def("add", &Phenomena::add,
            "add docstring...",
            py::return_value_policy::reference_internal)
    ;

    py::class_<Phenomenon>(module, "Phenomenon", py::base<hdf5::Group>(),
        "Phenomenon docstring...")
        .def("add_property_set",
                (PropertySet& (Phenomenon::*)
                    (std::string const&))
                        &Phenomenon::add_property_set,
            "add_property_set docstring...",
            "name"_a,
            py::return_value_policy::reference_internal)
        .def("add_property_set",
                (PropertySet& (Phenomenon::*)
                    (std::string const&, DomainConfiguration const&))
                        &Phenomenon::add_property_set,
            "add_property_set docstring...",
            "name"_a, "domain_configuration"_a,
            py::return_value_policy::reference_internal)
        .def_property_readonly("property_sets", &Phenomenon::property_sets,
            "property_sets docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace python
}  // namespace lue
