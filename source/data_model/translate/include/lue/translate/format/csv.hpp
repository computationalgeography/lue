#pragma once
#include "lue/dataset.hpp"
#include "lue/utility/metadata.hpp"


namespace lue {
    namespace utility {

        void translate_lue_dataset_to_csv(
            data_model::Dataset const& dataset, std::string const& csv_filename, Metadata const& metadata);

    }  // namespace utility
}  // namespace lue
