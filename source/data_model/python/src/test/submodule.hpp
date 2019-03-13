#pragma once


namespace pybind11 {

class module;

}  // namespace pybind11


namespace lue {
namespace test {

void               init_submodule      (pybind11::module& module);

}  // namespace test
}  // namespace lue
