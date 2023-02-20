#include "lue/info/property/same_shape/constant_shape/property.hpp"
#include "submodule.hpp"
#include <fmt/format.h>
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace constant_shape {
                namespace {

                    static std::string formal_string_representation(Property const& property)
                    {
                        return fmt::format("Property(pathname='{}')", property.id().pathname());
                    }


                    static std::string informal_string_representation(Property const& property)
                    {
                        return fmt::format("{}", formal_string_representation(property));
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

            }  // namespace constant_shape
        }      // namespace same_shape
    }          // namespace data_model
}  // namespace lue
