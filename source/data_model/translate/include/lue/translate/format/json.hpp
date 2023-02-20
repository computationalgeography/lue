#pragma once
#include "lue/data_model.hpp"
#include "lue/utility/metadata.hpp"
#include <nlohmann/json.hpp>


using json = nlohmann::json;

namespace lue {
    namespace utility {

        data_model::Dataset translate_json_to_lue(::json const& json, std::string const& dataset_name);

        void translate_json_to_lue(::json const& json, std::string const& lue_pathname, bool add);

        void translate_json_to_lue(
            std::string const& json_pathname,
            std::string const& lue_pathname,
            bool add,
            Metadata const& metadata);

    }  // namespace utility
}  // namespace lue
