#include "lue/phenomena.h"
#include "collection.h"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_phenomenon_class(
    py::module& module)
{

    BASE_COLLECTION(Phenomena, Phenomenon)


    py::class_<Phenomena, PhenomenonCollection>(
        module,
        "Phenomena",
        R"(
    Collection of LUE phenomena

    Zero of more Phenomenon instances can be stored in a Phenomenon
    collection.

    It is not possible to create Phenomena instances from scratch:
    a constructor is not defined. Phenomenon collections can be obtained
    from Dataset instances.
)")

        .def("add",
            &Phenomena::add,
            R"(
    Add new phenomenon to collection

    :param str name: Name of phenomenon to create
    :raises RuntimeError: In case the phenomenon cannot be created
)",
            "name"_a,
            py::return_value_policy::reference_internal)

        ;


    py::class_<Phenomenon, hdf5::Group>(
        module,
        "Phenomenon",
        R"(
    Phenomenon docstring...
)")

        .def(
            "__repr__",
            [](
                    Phenomenon const& phenomenon) {
                return "Phenomenon(pathname='" +
                    phenomenon.id().pathname() + "')";
            }
        )

//         // TODO This one is overwritten in __init__.py. This affects the
//         //      API documentation, which becomes useless.
//         .def(
//             "add_property_set",
//             (PropertySet& (Phenomenon::*)(std::string const&))
//                 &Phenomenon::add_property_set,
//             R"(
//     Add new property set to collection
// 
//     This property set's domain will be:
// 
//     - time: omnipresent
//     - space: omnipresent
// 
//     The return type of the property set will reflect the kind of the domain
//     that is configured.
// )",
//             "name"_a,
//             py::return_value_policy::reference_internal)
// 
//         .def(
//             "add_property_set",
//             [](
//                     Phenomenon& phenomenon, std::string const& name,
//                     PropertySetConfiguration const& configuration,
//                     SpaceDomainConfiguration const& space_domain_configuration)
//                         -> PropertySet& {
//                 return phenomenon.add_property_set(name, configuration,
//                     space_domain_configuration);
//             },
//             R"(
//     Add new property set to collection
// 
//     This property set's domain will be:
// 
//     - time: omnipresent
//     - space: according to configuration passed in
// 
//     The return type of the property set will reflect the kind of the domain
//     that is configured.
// )",
//             "name"_a,
//             "configuration"_a,
//             "space_domain_configuration"_a,
//             py::return_value_policy::reference_internal)
// 
//         .def(
//             "add_property_set",
//             (PropertySet& (Phenomenon::*) (std::string const&,
//                  PropertySetConfiguration const& configuration,
//                  DomainConfiguration const&)) &Phenomenon::add_property_set,
//             R"(
//     Add new property set to collection
// 
//     This property set's domain will be configured according to the
//     configuration passed in.
// 
//     The return type of the property set will reflect the kind of the domain
//     that is configured.
// )",
//             "name"_a,
//             "configuration"_a,
//             "domain_configuration"_a,
//             py::return_value_policy::reference_internal)

        .def_property_readonly(
            "property_sets",
            py::overload_cast<>(&Phenomenon::property_sets),
            R"(
    Return property sets collection

    :rtype: lue.PropertySets
)",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
