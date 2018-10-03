#include "lue/info/identity/active_object_index.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_active_object_index(
    py::module& module)
{

    py::class_<ActiveObjectIndex, same_shape::constant_shape::Value>(
        module,
        "ActiveObjectIndex",
        R"(
    ActiveObjectIndex docstring...
)")

        ;

}

}  // namespace lue
