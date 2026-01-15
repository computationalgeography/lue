#pragma once
#include "lue/data_model/hl/export.hpp"
#include <array>
#include <string>


namespace lue {

    LUE_DATA_MODEL_HL_EXPORT auto parse_array_pathname(std::string const& array_pathname)
        -> std::array<std::string, 4>;

    LUE_DATA_MODEL_HL_EXPORT auto parse_array_pathname3(std::string const& array_pathname)
        -> std::array<std::string, 3>;

}  // namespace lue
