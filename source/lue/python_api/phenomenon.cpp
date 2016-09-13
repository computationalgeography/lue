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
        R"(Collection of LUE phenomena

    Zero of more Phenomenon instances can be stored in a Phenomenon
    collection.

    It is not possible to create Phenomena instances from scratch:
    a constructor is not defined. Phenomenon collections can be obtained
    from Dataset instances.
)")
        .def("__repr__",
            [](Phenomena const& phenomena) {
                return "Phenomena(size=" + std::to_string(
                    phenomena.size()) + ")";
            }
        )
        .def("add", &Phenomena::add,
            R"(Add new phenomenon to collection

    :param str name: Name of phenomenon to create
    :raises RuntimeError: In case the phenomenon cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)
    ;


    py::class_<Phenomenon>(module, "Phenomenon", py::base<hdf5::Group>(),
        "Phenomenon docstring...")

        .def("__repr__",
            [](Phenomenon const& phenomenon) {
                return "Phenomenon(pathname='" +
                    phenomenon.id().pathname() + "')";
            }
        )
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
