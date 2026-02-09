#pragma once
#include "lue/core/time/epoch.hpp"
#include <string>


namespace lue::data_model {

    auto formal_string_representation(time::Epoch const& epoch) -> std::string;

    auto informal_string_representation(time::Epoch const& epoch) -> std::string;

}  // namespace lue::data_model
