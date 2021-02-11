#include "submodule.hpp"
#include "lue/data_model/hl/dataset_view.hpp"


using namespace pybind11::literals;


namespace lue::data_model {

    void bind_dataset_view(
        [[maybe_unused]] pybind11::module& module)
    {
        pybind11::class_<DatasetView<Dataset*>>(
            module,
            "DatasetView");
    }

}  // namespace lue::data_model
