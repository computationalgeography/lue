#pragma once
#include <pybind11/pybind11.h>


namespace lue::image_land {

    PYBIND11_EXPORT void init_submodule(pybind11::module& module);

}  // namespace lue::image_land
