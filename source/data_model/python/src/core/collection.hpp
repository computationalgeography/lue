#pragma once
#include "lue/core/collection.hpp"
#include <pybind11/stl.h>


namespace lue {
namespace python {

#define BASE_COLLECTION(Items, Item)                                       \
                                                                           \
    using Item##Collection = Collection<Item>;                             \
                                                                           \
    py::class_<Item##Collection, hdf5::Group>(                             \
        module,                                                            \
        #Item "Collection"                                                 \
)                                                                          \
                                                                           \
        .def(                                                              \
            "__repr__",                                                    \
            [](                                                            \
                    Item##Collection const& collection) {                  \
                return #Items "(size=" +                                   \
                    std::to_string(collection.size()) + ")";               \
            }                                                              \
        )                                                                  \
                                                                           \
        .def_property_readonly(                                            \
            "names",                                                       \
            &Item##Collection::names,                                      \
    "Return collection of names of objects in collection\n"                \
)                                                                          \
                                                                           \
        .def(                                                              \
            "__contains__",                                                \
            [](                                                            \
                    Item##Collection const& collection,                    \
                    std::string const& name) {                             \
                return collection.contains(name);                          \
            },                                                             \
    "Return whether or not the collection contains an object\n"            \
    "\n"                                                                   \
    ":param str name: Name of object to find\n",                           \
            "name"_a)                                                      \
                                                                           \
        .def(                                                              \
            "__getitem__",                                                 \
            py::overload_cast<std::string const&>(                         \
                &Item##Collection::operator[]),                            \
    "Return object\n"                                                      \
    "\n"                                                                   \
    ":param str name: Name of object to find\n"                            \
    ":raises RuntimeError: In case the collection does not contain the\n"  \
    "   object\n",                                                         \
            "name"_a,                                                      \
            py::return_value_policy::reference_internal)                   \
                                                                           \
        .def(                                                              \
            "__len__",                                                     \
            &Item##Collection::size,                                       \
    "Return the number of objects in the collection\n"                     \
    "\n"                                                                   \
    "...")                                                                 \
                                                                           \
        ;


}  // namespace python
}  // namespace lue
