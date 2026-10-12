#pragma once
#include "lue/utility/export.hpp"
#include <string>


namespace lue {

    LUE_UTILITY_EXPORT auto expand_environment_variables(std::string string) -> std::string;

}  // namespace lue
