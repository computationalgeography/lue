#pragma once
#include "lue/object/dataset.hpp"
#include <memory>


namespace lue::utility {

    /*!
        @brief      Unique pointer to a dataset
    */
    using LUEDatasetPtr = std::unique_ptr<data_model::Dataset>;


    auto try_open_lue_dataset_for_read(std::string const& dataset_name) -> LUEDatasetPtr;

}  // namespace lue::utility
