#include <pybind11/pybind11.h>
#include "lue/cxx_api/hello_world.h"


namespace py = pybind11;


namespace lue {
namespace python {

PYBIND11_PLUGIN(_lue)
{
    py::module module("_lue");

    py::class_<HelloWorld>(module, "HelloWorld")
        .def(py::init<>())
        .def("hello_world", &HelloWorld::hello_world)
        ;

    return module.ptr();
}

}  // namespace python
}  // namespace lue
