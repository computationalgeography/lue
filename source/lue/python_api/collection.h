#pragma once
#include "lue/cxx_api/collection.h"


namespace lue {
namespace python {

#define BASE_COLLECTION(Item)                                 \
                                                              \
    using Item##Collection = Collection<Item>;                \
                                                              \
    py::class_<Item##Collection>(module, #Item "Collection",  \
            py::base<hdf5::Group>(),                          \
        #Item "Collection docstring...")                      \
        .def("names", &Item##Collection::names,               \
            "names docstring...")                             \
        .def("item", &Item##Collection::item,                 \
            "item docstring...",                              \
            py::return_value_policy::reference_internal)      \
    ;

}  // namespace python
}  // namespace lue
