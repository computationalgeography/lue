#pragma once
#include "lue/utility/metadata.hpp"


namespace lue {
    namespace utility {

        void translate_geo_eas_to_lue(
            std::string const& geo_eas_pathname, std::string const& lue_pathname, Metadata const& metadata);

    }  // namespace utility
}  // namespace lue
