/// #include "property.hpp"
#include "lue/constant_size/time/located/shared/property_set.hpp"
// #include "lue/constant_size/time/located/shared/different_shape/property.hpp"
// #include "lue/constant_size/time/located/shared/same_shape/property.hpp"
#include "lue/phenomenon.hpp"
// #include <pybind11/stl.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

void init_property_set(
    py::module& module)
{

    py::class_<PropertySet, located::PropertySet>(
        module,
        "PropertySet",
        "PropertySet docstring...")

        .def(
            "reserve",
            &PropertySet::reserve,
            "reserve docstring...",
            py::return_value_policy::reference_internal)

        .def_property_readonly(
            "ids",
            py::overload_cast<>(&PropertySet::ids),
            "ids docstring...",
            py::return_value_policy::reference_internal)

        ;

        // .def_property_readonly(
        //     "domain",
        //     py::overload_cast<>(&PropertySet::domain),
        //     "domain docstring...",
        //     py::return_value_policy::reference_internal)

//         .def_property_readonly(
//             "property_names",
//             [](
//                     PropertySet const& property_set) {
//                 return property_set.properties().names();
//             },
//             "property_names docstring...")
// 
//         .def(
//             "__getitem__",
//             [](
//                 PropertySet& self,
//                 std::string const& name)
//             {
//                 return cast_to_specialized_property(
//                     self.properties()[name]);
//             },
//     "Return property\n"
//     "\n"
//     ":param str name: Name of property to find\n"
//     ":raises RuntimeError: In case the collection does not contain the\n"
//     "   property\n",
//             "name"_a
//         )


    module.def(
        "create_property_set",
        // py::overload_cast<Phenomenon&, std::string const&>(
        //     &create_property_set),
        [](
            Phenomenon& phenomenon,
            std::string const& name)
        {
            return create_property_set(phenomenon.property_sets(), name);
        },
        R"(
    Create new property set

    The property set will be added to the phenomenon
)",
        "phenomenon"_a,
        "name"_a,
        py::return_value_policy::move)

        ;

//     module.def(
//         "create_property_set",
//         py::overload_cast<
//             Phenomenon&, std::string const&, same_shape::Value const&>(
//                 &create_property_set),
//         R"(
//     Create new property set
// 
//     The property set will be added to the phenomenon
// )",
//         "phenomenon"_a,
//         "name"_a,
//         "ids"_a,
//         py::return_value_policy::move)
// 
//         ;

}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
