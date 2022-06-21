#pragma once
#include <pybind11/pybind11.h>


namespace lue::qa {

    PYBIND11_EXPORT void init_submodule(pybind11::module& module);

}  // namespace lue::qa
