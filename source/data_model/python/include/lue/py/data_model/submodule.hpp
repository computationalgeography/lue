#pragma once
#include <pybind11/pybind11.h>


namespace lue {
namespace data_model {

PYBIND11_EXPORT void
                   init_submodule      (pybind11::module& module);

}  // namespace data_model
}  // namespace lue
