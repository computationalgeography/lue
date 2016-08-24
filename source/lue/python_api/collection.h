#pragma once
#include "lue/cxx_api/collection.h"
#include <pybind11/stl.h>


namespace lue {
namespace python {

#define BASE_COLLECTION(Item)                                              \
                                                                           \
    using Item##Collection = Collection<Item>;                             \
                                                                           \
    py::class_<Item##Collection>(module, #Item "Collection",               \
            py::base<hdf5::Group>(),                                       \
        #Item "Collection docstring...")                                   \
        .def_property_readonly("names", &Item##Collection::names,          \
    "Return list of names of items in collection\n")                       \
        .def("__contains__", [](Item##Collection const& collection,        \
            std::string const& name) {                                     \
                return collection.contains(name);                          \
            },                                                             \
    "Return whether or not the collection contains an item\n"              \
    "\n"                                                                   \
    ":param str name: Name of item to find\n",                             \
            "name"_a)                                                      \
        .def("__getitem__", &Item##Collection::item,                       \
    "Return item\n"                                                        \
    "\n"                                                                   \
    ":param str name: Name of item to find\n"                              \
    ":raises RuntimeError: In case the collection does not contain the\n"  \
    "   item\n",                                                           \
            "name"_a,                                                      \
            py::return_value_policy::reference_internal)                   \
        .def("__size__", &Item##Collection::size,                          \
    "Return the number of items in the collection\n"                       \
    "\n"                                                                   \
    "...")                                                                 \
    ;


}  // namespace python
}  // namespace lue
