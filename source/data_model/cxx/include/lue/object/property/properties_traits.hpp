#pragma once
#include "lue/info/property/property_traits.hpp"


namespace lue::data_model {

    template<typename Properties>
    using PropertyT = typename PropertyTraits<Properties>::Property;

}  // namespace lue::data_model
