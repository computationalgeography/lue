#pragma once


namespace pybind11 {

class module;

}  // namespace pybind11


namespace lue {
namespace hdf5 {

void               init_dataset        (pybind11::module& module);

void               init_file           (pybind11::module& module);

void               init_group          (pybind11::module& module);

void               init_identifier     (pybind11::module& module);

void               init_shape          (pybind11::module& module);

void               init_submodule      (pybind11::module& module);

void               init_validate       (pybind11::module& module);

}  // namespace hdf5
}  // namespace lue
