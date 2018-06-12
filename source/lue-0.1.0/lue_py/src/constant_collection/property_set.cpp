#include "lue/constant_collection/property_set.hpp"
// #include "lue/constant_collection/time/omnipresent/different_shape/property.hpp"
// #include "lue/constant_collection/time/omnipresent/same_shape/property.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_collection {

void init_property_set_class(
    py::module& module)
{

    py::class_<PropertySet, lue::PropertySet>(
        module,
        "PropertySet",
        "PropertySet docstring...")

        .def_property_readonly(
            "ids",
            py::overload_cast<>(&PropertySet::ids),
            "ids docstring...",
            py::return_value_policy::reference_internal)






        // .def(
        //     py::init<lue::Phenomenon&, std::string const&>(),
        //     "__init__ docstring..."
        //     "phenomenon"_a,
        //     "name"_a,
        //     py::keep_alive<1, 2>())

        // .def_property_readonly(
        //     "domain",
        //     py::overload_cast<>(&PropertySet::domain),
        //     "domain docstring...",
        //     py::return_value_policy::reference_internal)

        // .def(
        //     "__getitem__",
        //     [](
        //         PropertySet& self,
        //         std::string const& name)
        //     {
        //         return cast_to_specialized_property(
        //             self.properties()[name]);
        //     },
        //     "Return property\n"
        //     "\n"
        //     ":param str name: Name of property to find\n"
        //     ":raises RuntimeError: In case the collection does not contain the\n"
        //     "   property\n",
        //             "name"_a
        // )

        ;

    module.def(
        "create_property_set",
        // py::overload_cast<Phenomenon&, std::string const&>(
        //     &create_property_set),
        [](
            Phenomenon& phenomenon,
            std::string const& name)
        {
            return create_property_set(phenomenon, name);
        },
        R"(
    Create new property set

    The property set will be added to the phenomenon
)",
        "phenomenon"_a,
        "name"_a,
        py::return_value_policy::move)

        ;

    module.def(
        "create_property_set",
        // py::overload_cast<
        //     Phenomenon&, std::string const&, same_shape::Value const&>(
        //         &create_property_set),
        [](
            Phenomenon& phenomenon,
            std::string const& name,
            PropertySet::Ids const& ids)
        {
            return create_property_set(phenomenon, name, ids);
        },
        R"(
    Create new property set

    The property set will be added to the phenomenon
)",
        "phenomenon"_a,
        "name"_a,
        "ids"_a,
        py::return_value_policy::move)

        ;
}

}  // namespace constant_collection
}  // namespace lue
