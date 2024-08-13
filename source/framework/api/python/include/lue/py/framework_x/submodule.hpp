#pragma once
#include <pybind11/pybind11.h>


namespace lue::api {

    PYBIND11_EXPORT void init_framework_x(pybind11::module& module);

}  // namespace lue::api
