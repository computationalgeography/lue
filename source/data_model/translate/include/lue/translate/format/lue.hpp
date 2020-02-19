#pragma once
#include "lue/object/dataset.hpp"
#include <memory>


namespace lue {
namespace utility {

/*!
    @brief      Unique pointer to a dataset
*/
using LUEDatasetPtr = std::unique_ptr<data_model::Dataset>;


LUEDatasetPtr      try_open_lue_dataset_for_read(
                                        std::string const& dataset_name);

}  // namespace utility
}  // namespace lue
