#include "collection.hpp"
#include "lue/property_sets.hpp"
#include "lue/time_domain.hpp"
#include "lue/constant_collection/property_set.hpp"
#include "lue/constant_collection/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_collection/time/omnipresent/same_shape/property.hpp"
#include "lue/constant_collection/time/located/constant_shape/different_shape/property.hpp"
#include "lue/constant_collection/time/located/constant_shape/same_shape/property.hpp"
#include <pybind11/pybind11.h>
#include <cassert>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_property_set_class(
    py::module& module)
{

    BASE_COLLECTION(PropertySets, PropertySet)


    py::class_<PropertySets, PropertySetCollection>(
        module,
        "PropertySets",
        R"(
    PropertySets docstring...
)")

        .def(
            "__getitem__",
            [](
                PropertySets& self,
                std::string const& name)
            {
                auto& property_set = self[name];
                auto const& configuration = property_set.configuration();

                py::object object = py::none{};

                switch(configuration.type<CollectionVariability>()) {
                    case CollectionVariability::constant: {
                        object = py::cast(new
                            constant_collection::PropertySet(
                                property_set.id()));
                        break;
                    }
                }

                return object;
            },
    "Return item\n"
    "\n"
    ":param str name: Name of item to find\n"
    ":raises RuntimeError: In case the collection does not contain the\n"
    "   item\n",
            "name"_a
        )

        // .def("__repr__",
        //     [](PropertySets const& property_sets) {
        //         return "PropertySets(size=" + std::to_string(
        //             property_sets.size()) + ")";
        //     }
        // )

        ;

    py::class_<PropertySet, hdf5::Group>(
        module,
        "PropertySet",
        "PropertySet docstring...")

        .def("__repr__",
            [](PropertySet const& property_set) {
                return "PropertySet(pathname='" +
                    property_set.id().pathname() + "')";
            }
        )

        .def_property_readonly(
            "name",
            [](PropertySet const& self)
            {
                return self.id().name();
            },
            "name docstring...")

        .def_property_readonly(
            "property_names",
            [](
                    PropertySet const& property_set) {
                return property_set.properties().names();
            },
            "property_names docstring...")

        .def(
            "__getitem__",
            [](
                PropertySet& self,
                std::string const& name)
            {
                auto& property = self.properties()[name];
                auto const& configuration = property.configuration();
                py::object object = py::none{};

                switch(configuration.type<ShapePerItem>()) {
                    case ShapePerItem::different: {
                        switch(configuration.type<Occurrence>()) {
                            case Occurrence::continuous: {
                                // different_shape
                                // continuous
                                assert(
                                    configuration.type<ShapeVariability>() ==
                                    ShapeVariability::constant);
                                assert(
                                    configuration.type<CollectionVariability>() ==
                                    CollectionVariability::constant);

        auto const file_datatype =
            constant_collection::time::omnipresent::different_shape::Property::file_datatype(property);
        object = py::cast(new
            constant_collection::time::omnipresent::different_shape::Property{
                hdf5::Group{property.id()},
                memory_datatype(file_datatype)});

                                break;
                            }
                            case Occurrence::asynchronous: {
                                // TODO
                                assert(false);
                                break;
                            }
                            case Occurrence::synchronous: {
                                switch(configuration.type<ShapeVariability>()) {
                                    case ShapeVariability::constant: {
                                        // different_shape
                                        // synchronous
                                        // constant_shape
                                        assert(
                                            configuration.type<CollectionVariability>() ==
                                            CollectionVariability::constant);

        auto const file_datatype =
            constant_collection::time::located::constant_shape::different_shape::Property::file_datatype(property);
        object = py::cast(new
            constant_collection::time::located::constant_shape::different_shape::Property{
                hdf5::Group{property.id()},
                memory_datatype(file_datatype)});

                                        break;
                                    }
                                }

                                break;
                            }
                        }

                        break;
                    }
                    case ShapePerItem::same: {
                        switch(configuration.type<Occurrence>()) {
                            case Occurrence::continuous: {
                                // same_shape
                                // continuous
                                assert(
                                    configuration.type<ShapeVariability>() ==
                                    ShapeVariability::constant);
                                assert(
                                    configuration.type<CollectionVariability>() ==
                                    CollectionVariability::constant);

        auto const file_datatype =
            constant_collection::time::omnipresent::same_shape::Property::file_datatype(property);
        object = py::cast(new
            constant_collection::time::omnipresent::same_shape::Property{
                hdf5::Group{property.id()},
                memory_datatype(file_datatype)});

                                break;
                            }
                            case Occurrence::asynchronous: {
                                // TODO
                                assert(false);
                                break;
                            }
                            case Occurrence::synchronous: {
                                switch(configuration.type<ShapeVariability>()) {
                                    case ShapeVariability::constant: {
                                        // same_shape
                                        // synchronous
                                        // constant_shape
                                        assert(
                                            configuration.type<CollectionVariability>() ==
                                            CollectionVariability::constant);

        auto const file_datatype =
            constant_collection::time::located::constant_shape::same_shape::Property::file_datatype(property);
        object = py::cast(new
            constant_collection::time::located::constant_shape::same_shape::Property{
                hdf5::Group{property.id()},
                memory_datatype(file_datatype)});

                                        break;
                                    }
                                }

                                break;
                            }
                        }

                        break;
                    }
                }

                return object;
            },
            "Return property\n"
            "\n"
            ":param str name: Name of property to find\n"
            ":raises RuntimeError: In case the collection does not contain the\n"
            "   property\n",
            "name"_a)

        .def_property_readonly(
            "domain",
            py::overload_cast<>(&PropertySet::domain, py::const_),
            "domain docstring...",
            py::return_value_policy::reference_internal)

        ;

}

}  // namespace lue
