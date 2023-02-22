#pragma once
#include "lue/object/dataset.hpp"
#include "lue/utility/metadata.hpp"
#include <ostream>


namespace lue {
    namespace utility {

        using DatasetRef = std::reference_wrapper<data_model::Dataset>;
        using DatasetRefs = std::vector<DatasetRef>;


        void translate_lue_dataset_to_dot(
            DatasetRefs const& datasets, std::string const& dot_filename, Metadata const& metadata);

        void translate_lue_dataset_to_dot(
            data_model::Dataset& dataset, std::ostream& stream, Metadata const& metadata);

        void translate_lue_dataset_to_dot(
            data_model::Dataset& dataset, std::string const& dot_filename, Metadata const& metadata);

    }  // namespace utility
}  // namespace lue
