#pragma once
#include "lue/object/dataset.hpp"
#include "lue/utility/metadata.hpp"


namespace lue::utility {

    void translate_netcdf4_dataset_to_lue(
        std::string const& input_dataset_name,
        std::string const& output_dataset_name,
        bool const append_data,
        Metadata const& metadata);

    void translate_lue_dataset_to_netcdf4(
        data_model::Dataset& dataset,
        std::string const& netcdf4_filename,
        Metadata const& metadata);

}  // namespace lue::utility
