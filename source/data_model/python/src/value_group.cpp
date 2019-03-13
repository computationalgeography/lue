#include "python_extension.hpp"
#include "lue/array/value_group.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {

void init_value_group(
    py::module& module)
{

    py::class_<ValueGroup, hdf5::Group>(
        module,
        "ValueGroup",
        R"(
    ValueGroup docstring...
)")

        ;

}

}  // namespace lue
