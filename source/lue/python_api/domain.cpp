#include "lue/cxx_api/domain.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace python {

void init_domain(
        py::module& module)
{
    py::enum_<TimeDomainType>(module, "time_domain",
        "time_domain docstring...")
        .value("omnipresent", TimeDomainType::omnipresent)
        .value("shared_constant_collection",
            TimeDomainType::shared_constant_collection)
        .value("shared_variable_collection",
            TimeDomainType::shared_variable_collection)
    ;

    py::enum_<SpaceDomainType>(module, "space_domain",
        "space_domain docstring...")
        .value("omnipresent", SpaceDomainType::omnipresent)
        .value("located", SpaceDomainType::located)
    ;

    py::enum_<SpaceDomainItemType>(module, "space_domain_item",
        "space_domain item docstring...")
        .value("none", SpaceDomainItemType::none)
        .value("point", SpaceDomainItemType::point)
        .value("box", SpaceDomainItemType::box)
        .value("line", SpaceDomainItemType::line)
        .value("region", SpaceDomainItemType::region)
        .value("cell", SpaceDomainItemType::cell)
    ;

    py::class_<TimeDomainConfiguration>(module, "TimeDomainConfiguration",
        "TimeDomainConfiguration docstring...")
        .def(py::init<>(),
            "__init__ docstring...")
        .def(py::init<TimeDomainType const&>(),
            "__init__ docstring...",
            "type"_a)
        .def_property_readonly("type", &TimeDomainConfiguration::type,
            "type docstring...")
    ;

    py::class_<SpaceDomainConfiguration>(module, "SpaceDomainConfiguration",
        "SpaceDomainConfiguration docstring...")
        .def(py::init<>(),
            "__init__ docstring...")
        .def(py::init<SpaceDomainItemType const&>(),
            "__init__ docstring...",
            "item_type"_a)
        .def(py::init<SpaceDomainType const&, SpaceDomainItemType const&>(),
            "__init__ docstring...",
            "type"_a,
            "item_type"_a)
        .def_property_readonly("type", &SpaceDomainConfiguration::type,
            "type docstring...")
        .def_property_readonly("item_type",
            &SpaceDomainConfiguration::item_type,
            "item type docstring...")
    ;

    py::class_<DomainConfiguration>(module, "DomainConfiguration",
        "DomainConfiguration docstring...")
        .def(py::init<>(),
            "__init__ docstring...")
        .def(py::init<
                TimeDomainConfiguration const&>(),
            "__init__ docstring..."
            "time"_a)
        .def(py::init<
                SpaceDomainConfiguration const&>(),
            "__init__ docstring..."
            "space"_a)
        .def(py::init<
                TimeDomainConfiguration const&,
                SpaceDomainConfiguration const&>(),
            "__init__ docstring..."
            "time"_a, "space"_a)
        .def_property_readonly("time", &DomainConfiguration::time,
            "time docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("space", &DomainConfiguration::space,
            "space docstring...",
            py::return_value_policy::reference_internal)
    ;

    py::class_<TimeDomain>(module, "TimeDomain", py::base<hdf5::Group>(),
        "TimeDomain docstring...")
        .def_property_readonly("configuration", &TimeDomain::configuration,
            "configuration docstring...",
            py::return_value_policy::reference_internal)
    ;

    py::class_<SpaceDomain>(module, "SpaceDomain", py::base<hdf5::Group>(),
        "SpaceDomain docstring...")
        .def_property_readonly("configuration", &SpaceDomain::configuration,
            "configuration docstring...",
            py::return_value_policy::reference_internal)
    ;

    py::class_<Domain>(module, "Domain", py::base<hdf5::Group>(),
        "Domain docstring...")
        .def_property_readonly("configuration", &Domain::configuration,
            "configuration docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("time_domain", &Domain::time_domain,
            "time docstring...",
            py::return_value_policy::reference_internal)
        .def_property_readonly("space_domain", &Domain::space_domain,
            "space docstring...",
            py::return_value_policy::reference_internal)
    ;

}

}  // namespace python
}  // namespace lue
