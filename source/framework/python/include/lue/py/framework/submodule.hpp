#pragma once
#include <pybind11/pybind11.h>


namespace lue {
namespace framework {

PYBIND11_EXPORT void
                   init_submodule      (pybind11::module& module);

}  // namespace framework
}  // namespace lue
