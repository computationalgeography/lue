#include "lue/info/property/different_shape/property.hpp"
#include "submodule.hpp"
#include <pybind11/pybind11.h>
#include <format>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {
        namespace different_shape {
            namespace {

                static std::string formal_string_representation(Property const& property)
                {
                    return std::format("Property(pathname='{}')", property.id().pathname());
                }


                static std::string informal_string_representation(Property const& property)
                {
                    return std::format("{}", formal_string_representation(property));
                }

            }  // Anonymous namespace


            void init_property(py::module& module)
            {

                py::class_<Property, PropertyGroup>(
                    module,
                    "Property",
                    R"(
    Property docstring...
)")

                    .def(
                        "__repr__",
                        [](Property const& property) { return formal_string_representation(property); })

                    .def(
                        "__str__",
                        [](Property const& property) { return informal_string_representation(property); })

                    .def_property_readonly(
                        "value",
                        py::overload_cast<>(&Property::value),
                        py::return_value_policy::reference_internal)

                    ;
            }

        }  // namespace different_shape
    }  // namespace data_model
}  // namespace lue
