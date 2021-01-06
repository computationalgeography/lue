#include "submodule.hpp"
#include "constant_shape/submodule.hpp"
#include "variable_shape/submodule.hpp"


namespace py = pybind11;


namespace lue {
namespace data_model {
namespace different_shape {

void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "different_shape",
        R"(
    :mod:`lue.data_model.different_shape` --- Object arrays
    =======================================================

    The :mod:`lue.data_model.different_shape` package contains functionality for
    manipulating object arrays with different shapes.

    .. automodule:: lue.data_model.different_shape.constant_shape
    .. automodule:: lue.data_model.different_shape.variable_shape
)");

    init_properties(submodule);
    init_property(submodule);
    init_value(submodule);

    constant_shape::init_submodule(submodule);
    variable_shape::init_submodule(submodule);
}

}  // namespace different_shape
}  // namespace data_model
}  // namespace lue
