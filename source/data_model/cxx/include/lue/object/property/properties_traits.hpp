#pragma once
#include "lue/info/property/property_traits.hpp"


namespace lue {
namespace data_model {

// template<
//     typename Properties>
// class PropertiesTraits
// {
// };


template<
    typename Properties>
using PropertyT = typename PropertyTraits<Properties>::Property;


// template<
//     typename Properties>
// using ValueT = typename PropertiesTraits<Properties>::Value;

}  // namespace data_model
}  // namespace lue
