#include "../python_extension.hpp"
#include "lue/info/identity/object_id.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_object_id(
    py::module& module)
{

    py::class_<ObjectID, same_shape::Value>(
        module,
        "ObjectID",
        R"(
    ObjectID docstring...
)")

        ;

}

}  // namespace lue
