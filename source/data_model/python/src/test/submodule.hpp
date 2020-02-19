#pragma once


namespace pybind11 {

class module;

}  // namespace pybind11


namespace lue {
namespace data_model {
namespace test {

void               init_submodule      (pybind11::module& module);

}  // namespace test
}  // namespace data_model
}  // namespace lue
