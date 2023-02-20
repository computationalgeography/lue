#pragma once
#include <pybind11/pybind11.h>


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace variable_shape {

                void init_properties(pybind11::module& module);

                void init_property(pybind11::module& module);

                void init_submodule(pybind11::module& module);

                void init_value(pybind11::module& module);

            }  // namespace variable_shape
        }      // namespace same_shape
    }          // namespace data_model
}  // namespace lue
