#include "lue/info/identity/active_set_index.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_active_set_index(
    py::module& module)
{

    py::class_<info::ActiveSetIndex, same_shape::constant_shape::Value>(
        module,
        "ActiveSetIndex",
        R"(
    ActiveSetIndex docstring...
)")

        ;

}

}  // namespace lue
