#include "submodule.hpp"


namespace py = pybind11;


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace variable_shape {

                void init_properties(py::module& module);
                void init_property(py::module& module);
                void init_value(py::module& module);


                void init_submodule(py::module& module)
                {
                    py::module submodule = module.def_submodule(
                        "variable_shape",
                        R"(
    :mod:`lue.data_model.same_shape.variable_shape` --- Object arrays
    =================================================================

    The :mod:`lue.data_model.same_shape.variable_shape` package contains
    functionality for manipulating object arrays with the same shapes
    that (the shapes) are variable through time.
)");

                    init_properties(submodule);
                    init_property(submodule);
                    init_value(submodule);
                }

            }  // namespace variable_shape
        }      // namespace same_shape
    }          // namespace data_model
}  // namespace lue
