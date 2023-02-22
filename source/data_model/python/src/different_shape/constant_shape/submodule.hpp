#pragma once
#include <pybind11/pybind11.h>


namespace lue {
    namespace data_model {
        namespace different_shape {
            namespace constant_shape {

                void init_properties(pybind11::module& module);

                void init_property(pybind11::module& module);

                void init_submodule(pybind11::module& module);

                void init_value(pybind11::module& module);

            }  // namespace constant_shape
        }      // namespace different_shape
    }          // namespace data_model
}  // namespace lue
