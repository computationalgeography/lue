#pragma once
#include <pybind11/pybind11.h>


namespace lue::framework {

    PYBIND11_EXPORT void init_submodule(pybind11::module& module);

}  // namespace lue::framework
