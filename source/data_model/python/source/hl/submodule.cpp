#include "submodule.hpp"
#include "lue/data_model/hl/raster_view.hpp"


namespace lue::data_model {

    void bind_hl_submodule(pybind11::module& module)
    {
        pybind11::module submodule = module.def_submodule(
            "hl",
            R"(
    :mod:`lue.data_model.hl` --- High level data-model API
    ======================================================

    ...
    )");

        bind_dataset_view(submodule);
        bind_raster_view(submodule);
    }

}  // namespace lue::data_model
