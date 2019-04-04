#pragma once
#include "lue/core/time/epoch.hpp"
#include <string>


namespace lue {

std::string        formal_string_representation(
                                        time::Epoch const& epoch);

std::string        informal_string_representation(
                                        time::Epoch const& epoch);

}  // namespace lue
