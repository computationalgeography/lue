#pragma once
#include <pybind11/pybind11.h>


namespace lue {
namespace data_model {
namespace test {

void               init_submodule      (pybind11::module& module);

}  // namespace test
}  // namespace data_model
}  // namespace lue
