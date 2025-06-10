#pragma once
#include <pybind11/pybind11.h>


namespace lue::hdf5 {

    void init_dataset(pybind11::module& module);

    void init_file(pybind11::module& module);

    void init_group(pybind11::module& module);

    void init_identifier(pybind11::module& module);

    void init_primary_data_object(pybind11::module& module);

    void init_shape(pybind11::module& module);

    void init_submodule(pybind11::module& module);

    void init_validate(pybind11::module& module);

} // namespace lue::hdf5
