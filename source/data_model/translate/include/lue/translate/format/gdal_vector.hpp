#pragma once
#include "lue/object/dataset.hpp"
#include "lue/utility/metadata.hpp"
#include <ogrsf_frmts.h>


namespace lue::utility {

    void translate_lue_dataset_to_shapefile(
        data_model::Dataset& dataset, std::string const& shapefile_name, Metadata const& metadata);

}  // namespace lue::utility
