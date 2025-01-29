#pragma once
#include <pybind11/pybind11.h>


namespace lue::data_model {

    void bind_dataset_view(pybind11::module& module);

    void bind_hl_submodule(pybind11::module& module);

    void bind_raster_view(pybind11::module& module);

}  // namespace lue::data_model
