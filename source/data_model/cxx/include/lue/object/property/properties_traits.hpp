#pragma once
#include "lue/info/property/property_traits.hpp"


namespace lue {
namespace data_model {

template<
    typename Properties>
using PropertyT = typename PropertyTraits<Properties>::Property;

}  // namespace data_model
}  // namespace lue
