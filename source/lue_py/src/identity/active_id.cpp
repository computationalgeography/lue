#include "lue/info/identity/active_id.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_active_id(
    py::module& module)
{

    py::class_<info::ActiveID, same_shape::constant_shape::Value>(
        module,
        "ActiveID",
        R"(
    ActiveID docstring...
)")

        ;

}

}  // namespace lue
