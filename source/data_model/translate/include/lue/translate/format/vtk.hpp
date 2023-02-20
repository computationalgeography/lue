#pragma once
#include "lue/dataset.hpp"
#include "lue/utility/metadata.hpp"


namespace lue {
    namespace utility {

        void translate_lue_dataset_to_vtk(
            data_model::Dataset const& dataset, std::string const& vtk_filename, Metadata const& metadata);

    }  // namespace utility
}  // namespace lue
