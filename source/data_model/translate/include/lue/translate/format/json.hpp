#pragma once
#include "lue/utility/metadata.hpp"


namespace lue {
namespace utility {

void               translate_json_to_lue(
                                        std::string const& json_pathname,
                                        std::string const& lue_pathname,
                                        bool add,
                                        Metadata const& metadata);

}  // namespace utility
}  // namespace lue
