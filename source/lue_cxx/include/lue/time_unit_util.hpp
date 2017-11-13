#pragma once
#include "lue/time/unit.hpp"
#include <string>


namespace lue {

std::string        unit_to_string      (time::Unit const unit);

time::Unit         string_to_unit      (std::string const& string);

}  // namespace lue
